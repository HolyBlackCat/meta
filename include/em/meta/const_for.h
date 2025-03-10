#pragma once

#include "em/macros/utils/forward.h"
#include "em/meta/cvref.h"
#include "em/meta/deduce.h"
#include "em/meta/lists.h"

#include <concepts>
#include <utility>

namespace em::Meta
{
    // We have some user-facing functions below, and each of them needs a `LoopBackend` (strategy) parameter
    //   that must be choosen from One of the `Loop...` classes below.

    struct BasicLoopBackend {};
    template <typename T>
    concept LoopBackendType = cvref_unqualified<T> && std::derived_from<T, BasicLoopBackend> && !std::same_as<T, BasicLoopBackend>;

    // Returns the empty element of this backend.
    template <LoopBackendType LoopBackend>
    [[nodiscard]] constexpr decltype(auto) NoElements() {return LoopBackend::NoElements();}


    // Foreach loops over types or values:
    // User lambdas must be `[...]<typename|auto|type I>{...}`.

    template <LoopBackendType LoopBackend, typename ...I>
    constexpr decltype(auto) ConstForEach(auto &&func) {return LoopBackend::template ForEach<I...>(EM_FWD(func));}
    template <LoopBackendType LoopBackend, auto ...I> requires (sizeof...(I) > 0) // Empty back is handled by the previous overload.
    constexpr decltype(auto) ConstForEach(auto &&func) {return LoopBackend::template ForEach<I...>(EM_FWD(func));}
    template <LoopBackendType LoopBackend, Meta::Deduce..., typename ...I>
    constexpr decltype(auto) ConstForEach(TypeList<I...>, auto &&func) {return LoopBackend::template ForEach<I...>(EM_FWD(func));}
    template <LoopBackendType LoopBackend, Meta::Deduce..., auto ...I>
    constexpr decltype(auto) ConstForEach(ValueList<I...>, auto &&func) {return LoopBackend::template ForEach<I...>(EM_FWD(func));}
    template <LoopBackendType LoopBackend, Meta::Deduce..., typename T, T ...I>
    constexpr decltype(auto) ConstForEach(std::integer_sequence<T, I...>, auto &&func) {return LoopBackend::template ForEach<I...>(EM_FWD(func));}

    // A for loop up to a specific size:
    // User lambda must be `[...]<auto|type I>{...}`.
    template <LoopBackendType LoopBackend, auto N>
    constexpr decltype(auto) ConstFor(auto &&func) {return (ConstForEach<LoopBackend>)(std::make_integer_sequence<decltype(N), N>{}, EM_FWD(func));}

    // This runs all functions you pass in order, with no arguments.
    template <LoopBackendType LoopBackend, Meta::Deduce...>
    constexpr decltype(auto) RunEachFunc(auto &&... funcs) {return LoopBackend::RunEachFunc(EM_FWD(funcs)...);}

    // Range iteration:
    // Note that this doesn't work with `LoopAnyOfConsteval`.
    // User lambda must be `[...](type elem){...}`.
    // NOTE! We're intentionally not adding clever overloads here for C++20 ranges or whatever.
    // Our reflection library handles that better than we could here (with optional element forwarding and all that).

    // Iterate over a range.
    template <LoopBackendType LoopBackend, Meta::Deduce...>
    constexpr decltype(auto) ForEach(auto begin, auto end, auto &&func) {return LoopBackend::ForRange(std::move(begin), std::move(end), EM_FWD(func));}


    // Loop strategies:
    // Pass those as template argument to the function above.
    // Each strategy has a reverse version, and you can switch between them using the `::reverse` typedef.

    // Forward declarations.
    struct LoopSimple_Reverse;
    template <typename ReturnType /*=...*/> struct LoopAnyOf_Reverse;
    template <typename ReturnTypeIfEmpty /*=...*/> struct LoopAnyOfConsteval_Reverse;

    // The simplest loop. Always returns void, can't be stopped midway. The return values of the functors are ignored.
    struct LoopSimple : BasicLoopBackend
    {
        static constexpr bool is_reverse = false;
        using reverse = LoopSimple_Reverse;

        static constexpr void NoElements() {}

        // The second overload only works for non-empty packs, because the first one handles empty non-type lists too.
        template <typename ...I>                            static constexpr void ForEach(auto &&func) {(void)(void(func.template operator()<I>()), ...);}
        template <auto     ...I> requires(sizeof...(I) > 0) static constexpr void ForEach(auto &&func) {(void)(void(func.template operator()<I>()), ...);}

        static constexpr void RunEachFunc(auto &&... funcs) {(void)(void(EM_FWD(funcs)()), ...);}

        static constexpr void ForRange(auto begin, auto end, auto &&func) {while (begin != end) {func(*begin); ++begin;}}
    };
    struct LoopSimple_Reverse : BasicLoopBackend
    {
        static constexpr bool is_reverse = true;
        using reverse = LoopSimple;

        static constexpr void NoElements() {}

        // The second overload only works for non-empty packs, because the first one handles empty non-type lists too.
        template <typename ...I>                            static constexpr void ForEach(auto &&func) {int x = 0; (void)((void(func.template operator()<I>()), x) = ... = 0);}
        template <auto     ...I> requires(sizeof...(I) > 0) static constexpr void ForEach(auto &&func) {int x = 0; (void)((void(func.template operator()<I>()), x) = ... = 0);}

        static constexpr void RunEachFunc(auto &&... funcs) {int x = 0; (void)((void(EM_FWD(funcs)()), x) = ... = 0);}

        static constexpr void ForRange(auto begin, auto end, auto &&func) {while (begin != end) {--end; func(*end);}}
    };

    // Always returns a fixed type, which must be default-constructible and assignable.
    // If the user functor returns TRUTHY, immediately stops and returns that value. Otherwise returns the result from the last iteration,
    //   or a default-constructed instance if there were no iterations.
    // In any case, all iterations get instantiated.
    // There is no "all of" variant because then it's unclear what the default return value should be (there's no way to expect truthiness
    //   for an arbitrary return type).
    template <typename ReturnType = bool>
    struct LoopAnyOf : BasicLoopBackend
    {
        static constexpr bool is_reverse = false;
        using reverse = LoopAnyOf_Reverse<ReturnType>;

        static constexpr ReturnType NoElements() {return ReturnType();}

        template <typename ...I>                            static constexpr ReturnType ForEach(auto &&func) {ReturnType ret{}; (void)(bool(ret = func.template operator()<I>()) || ...); return ret;}
        template <auto     ...I> requires(sizeof...(I) > 0) static constexpr ReturnType ForEach(auto &&func) {ReturnType ret{}; (void)(bool(ret = func.template operator()<I>()) || ...); return ret;}

        static constexpr ReturnType RunEachFunc(auto &&... funcs) {ReturnType ret{}; (void)(bool(ret = EM_FWD(funcs)()) || ...); return ret;}

        static constexpr ReturnType ForRange(auto begin, auto end, auto &&func) {ReturnType ret{}; while (begin != end) {if ((ret = func(*begin))) break; ++begin;} return ret;}
    };
    template <typename ReturnType = bool>
    struct LoopAnyOf_Reverse : BasicLoopBackend
    {
        static constexpr bool is_reverse = true;
        using reverse = LoopAnyOf<ReturnType>;

        static constexpr ReturnType NoElements() {return ReturnType();}

        template <typename ...I>                            static constexpr ReturnType ForEach(auto &&func) {return ConstForEach<reverse>(list_reverse<TypeList <I...>>{}, EM_FWD(func));}
        template <auto     ...I> requires(sizeof...(I) > 0) static constexpr ReturnType ForEach(auto &&func) {return ConstForEach<reverse>(list_reverse<ValueList<I...>>{}, EM_FWD(func));}

        static constexpr ReturnType RunEachFunc(auto &&... funcs) {return [&]<std::size_t ...I>(std::index_sequence<I...>){ReturnType ret{}; (void)(bool(ret = EM_FWD(funcs...[sizeof...(I)-1-I])()) || ...); return ret;}(std::make_index_sequence<sizeof...(funcs)>{});}

        static constexpr ReturnType ForRange(auto begin, auto end, auto &&func) {ReturnType ret{}; while (begin != end) {--end; if ((ret = func(*end))) break;} return ret;}
    };

    // This lets the final type to depend on the user return values.
    // Requires the user function to be constexpr and stateless. Only runs at compile-time.
    // Forwards the return type from the user functor exactly. If the functor returns TRUTHY,
    //   stops immediately and doesn't instantiate or run the remaining iterations, and returns that value.
    // If none of the iterations return TRUTHY, returns the last value or if no iterations a default-constructed instance of `ReturnTypeIfEmpty`,
    //   which can be void.
    // There is no "all of" variant for consistency with `LoopAnyOf`, see the comment on that for the explanation. Also we wouldn't really
    //   be able to keep using `std::nullptr_t` as the default return type, because we'd want something truthy by default.
    // The return value is always a non-reference, for simplicity.
    // This strategy can't be used to iterate over ranges.
    template <typename ReturnTypeIfEmpty = std::nullptr_t>
    struct LoopAnyOfConsteval : BasicLoopBackend
    {
        // This backend passes functions by value, otherwise things are not `constexpr` enough.
        //   We need this because we use `if constexpr` internally, because the return type needs to depend on the return value.
        // This is fixed in newer C++ (in Clang 20).
        // This doesn't really matter, because it only runs at compile-time and normally needs captureless lambdas.

        static constexpr bool is_reverse = false;
        using reverse = LoopAnyOfConsteval_Reverse<ReturnTypeIfEmpty>;

        static constexpr ReturnTypeIfEmpty NoElements() {return ReturnTypeIfEmpty();}

        template <typename F>
        static consteval ReturnTypeIfEmpty ForEach(F &&) {return ReturnTypeIfEmpty();} // `()` allows void to be returned.

        template <typename I0, typename ...I, typename F>
        static consteval auto ForEach(F func)
        {
            if constexpr (sizeof...(I) == 0)
            {
                return std::move(func).template operator()<I0>();
            }
            else
            {
                constexpr auto ret = func.template operator()<I0>();
                if constexpr (ret)
                    return ret;
                else
                    return (ForEach<I...>)(std::move(func));
            }
        }

        template <auto I0, auto ...I, typename F>
        static consteval auto ForEach(F func)
        {
            if constexpr (sizeof...(I) == 0)
            {
                return std::move(func).template operator()<I0>();
            }
            else
            {
                constexpr auto ret = func.template operator()<I0>();
                if constexpr (ret)
                    return ret;
                else
                    return (ForEach<I...>)(std::move(func));
            }
        }

        static consteval ReturnTypeIfEmpty RunEachFunc() {return ReturnTypeIfEmpty();} // `()` allows void to be returned.

        template <typename F0, typename ...F>
        static consteval auto RunEachFunc(F0 func, F ...funcs)
        {
            if constexpr (sizeof...(F) == 0)
            {
                return std::move(func)();
            }
            else
            {
                constexpr auto ret = std::move(func)();
                if constexpr (ret)
                    return ret;
                else
                    return (RunEachFunc)(std::move(funcs)...);
            }
        }

        // No `ForRange`, it seems to be impossible to implement for this loop strategy.
    };

    template <typename ReturnTypeIfEmpty = std::nullptr_t>
    struct LoopAnyOfConsteval_Reverse : BasicLoopBackend
    {
        // This backend passes functions by value, otherwise things are not `constexpr` enough.
        //   We need this because we use `if constexpr` internally, because the return type needs to depend on the return value.
        // This is fixed in newer C++ (in Clang 20).
        // This doesn't really matter, because it only runs at compile-time and normally needs captureless lambdas.

        static constexpr bool is_reverse = true;
        using reverse = LoopAnyOfConsteval<ReturnTypeIfEmpty>;

        static constexpr ReturnTypeIfEmpty NoElements() {return ReturnTypeIfEmpty();}

        template <typename ...I>                            static constexpr decltype(auto) ForEach(auto func) {return ConstForEach<reverse>(list_reverse<TypeList <I...>>{}, std::move(func));}
        template <auto     ...I> requires(sizeof...(I) > 0) static constexpr decltype(auto) ForEach(auto func) {return ConstForEach<reverse>(list_reverse<ValueList<I...>>{}, std::move(func));}

        static constexpr decltype(auto) RunEachFunc(auto ...funcs) {return [&]<std::size_t ...I>(std::index_sequence<I...>){return reverse::RunEachFunc(std::move(funcs...[sizeof...(I)-1-I])...);}(std::make_index_sequence<sizeof...(funcs)>{});}

        // No `ForRange`, it seems to be impossible to implement for this loop strategy.
    };
}
