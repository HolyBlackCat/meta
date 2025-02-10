#pragma once

#include "em/macros/meta/eval.h"
#include "em/macros/utils/forward.h"
#include "em/macros/utils/if_maybe_constexpr.h"
#include "em/macros/utils/lift.h"
#include "em/meta/deduce.h"
#include "em/meta/lists.h"
#include "em/meta/void.h"

#include <utility>

namespace em::Meta
{
    // Loop functions below return `NoElements<false>` if there are no elements to iterate.
    // There's a special behavior when the iteration function returns this. If `Value = true`, stops the function immediately, as for any truthy value.
    //   Then regardless of `Value`, backtracks to the preceding iterations and returns an old return value that's not a `NoElements<...>`.
    //   If all iterations returned `NoElements<...>`, then returns the result of the first iteration.
    template <bool Value>
    struct NoElements
    {
        [[nodiscard]] explicit constexpr operator bool() const {return Value;}
    };


    namespace detail
    {
        template <typename T> struct IsNoElementsImpl : std::false_type {};
        template <bool Value> struct IsNoElementsImpl<NoElements<Value>> : std::true_type {};
    }

    // Returns true if `T` is a `NoElements<...>`, ignoring cvref-qualifiers.
    template <typename T>
    concept IsNoElements = detail::IsNoElementsImpl<std::remove_cvref_t<T>>::value;


    namespace detail
    {
        EM_EVAL(
            EM_FOREACH_A( (typename)(auto) )
            (
                template <EM_A I0, EM_A ...I>
                constexpr decltype(auto) ConstForEx(auto &&func)
                {
                    if constexpr (sizeof...(I) == 0)
                    {
                        // Last call, so we can forward the func.
                        return EM_FWD(func).template operator()<I0>();
                    }
                    else
                    {
                        decltype(auto) ret = InvokeWithVoidPlaceholderResult(EM_EXPR(func.template operator()<I0>()));
                        EM_IF_ELSE_MAYBE_CONSTEXPR(
                            (VoidTo<false>)(ret),
                            (
                                EM_RETURN_VARIABLE(ret);
                            ),
                            (
                                decltype(auto) ret2 = InvokeWithVoidPlaceholderResult(EM_EXPR((ConstForEx<I...>)(EM_FWD(func))));
                                if constexpr (IsNoElements<decltype(ret2)>)
                                    EM_RETURN_VARIABLE(ret);
                                else
                                    EM_RETURN_VARIABLE(ret2);
                            )
                        )
                    }
                }
            )
        )
    }

    // An advanced constexpr `for` loop.
    // `func` is `[]<Type I>{...}`, calls it for `0..N-1`.
    // If `func` returns non-void and truthy, stops immediately and returns that result.
    // Otherwise runs all iterations and returns the result of the last iteration, or void if no iterations.
    // If the result of `func()` is constexpr-convertible to `bool`, the return type can depend on `I`. Otherwise it can't.
    //
    // So e.g. you can do `if constexpr (...) return std::true_type{};` (else don't return at all), will optimally stop the loop.
    //
    // Returns `NoElements<false>` if `N == 0`.
    // If the user callback returns `NoElements<...>`, backtracks to previous return values
    //   until it finds one that's not a `NoElements<...>`, or returns the first result if all of them are.
    // `NoElements<true>` is truthy and will stop the loop like any other value does, and `NoElements<false>` isn't.
    template <auto N, Meta::Deduce...>
    [[nodiscard]] constexpr decltype(auto) ConstForEx(auto &&func)
    {
        if constexpr (N <= 0)
        {
            return NoElements<false>{};
        }
        else
        {
            return [&]<decltype(N) ...I>(std::integer_sequence<decltype(N), I...>) -> decltype(auto)
            {
                return detail::ConstForEx<I...>(EM_FWD(func));
            }(std::make_integer_sequence<decltype(N), N>{});
        }
    }

    // Same but for-each over a type list.
    template <typename ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachEx(TypeList<T...>, auto &&func)
    {
        if constexpr (sizeof...(T) == 0)
            return NoElements<false>{};
        else
            return detail::ConstForEx<T...>(EM_FWD(func));
    }
    // Same but for-each over a value list.
    template <auto ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachEx(ValueList<T...>, auto &&func)
    {
        if constexpr (sizeof...(T) == 0)
            return NoElements<false>{};
        else
            return detail::ConstForEx<T...>(EM_FWD(func));
    }

    // Same but for-each over a type pack.
    template <typename ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachEx(auto &&func)
    {
        if constexpr (sizeof...(T) == 0)
            return NoElements<false>{};
        else
            return detail::ConstForEx<T...>(EM_FWD(func));
    }
    // Same but for-each over a value pack. (this overload rejects empty packs, but they are handled by the previous overload)
    template <auto T0, auto ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachEx(auto &&func)
    {
        return detail::ConstForEx<T0, T...>(EM_FWD(func));
    }



    namespace detail
    {
        template <Meta::Deduce..., typename F0, typename ...F>
        [[nodiscard]] constexpr decltype(auto) RunEachLambdaEx(F0 &&func0, F &&... func)
        {
            if constexpr (sizeof...(F) == 0)
            {
                // Last call, so we can forward the func.
                return EM_FWD(func0)();
            }
            else
            {
                decltype(auto) ret = InvokeWithVoidPlaceholderResult(func0);
                EM_IF_ELSE_MAYBE_CONSTEXPR(
                    (VoidTo<false>)(ret),
                    (
                        EM_RETURN_VARIABLE(ret);
                    ),
                    (
                        decltype(auto) ret2 = InvokeWithVoidPlaceholderResult(EM_EXPR((RunEachLambdaEx)(EM_FWD(func)...)));
                        if constexpr (IsNoElements<decltype(ret2)>)
                            EM_RETURN_VARIABLE(ret);
                        else
                            EM_RETURN_VARIABLE(ret2);
                    )
                )
            }
        }
    }

    // Calls each lambda in order, stops when any of them returns non-void and truthy.
    // Other than that, behaves like `ConstForEx()`.
    template <Meta::Deduce..., typename ...F>
    [[nodiscard]] constexpr decltype(auto) RunEachLambdaEx(F &&... func)
    {
        if constexpr (sizeof...(F) == 0)
            return NoElements<false>{};
        else
            return detail::RunEachLambdaEx(EM_FWD(func)...);
    }
}
