#pragma once

#include "em/macros/portable/tiny_func.h"
#include "em/macros/utils/flag_enum.h"
#include "em/macros/utils/forward.h"
#include "em/macros/utils/returns.h"
#include "em/meta/common.h"

#include <functional>
#include <type_traits>

namespace em::Meta
{
    // Like `std::invoke()`, but triggers a SFINAE error if the return type isn't `void`.
    // Note that `std::invoke_r<void>()` isn't the same thing, because that silently ignores return values and always returns `void` regardless.
    template <Deduce..., typename F, typename ...P> requires std::is_void_v<std::invoke_result_t<F &&, P &&...>>
    constexpr void invoke_void(F &&func, P &&... params) noexcept(std::is_nothrow_invocable_v<F &&, P &&...>)
    {
        std::invoke(EM_FWD(func), EM_FWD(params)...);
    }


    template <typename T>
    struct construct_t
    {
        template <typename ...P>
        [[nodiscard]] static constexpr T operator()(P &&... params)
        noexcept(std::is_nothrow_constructible_v<T, P &&...>)
        requires std::is_constructible_v<T, P &&...>
        {
            return T(decltype(params)(params)...);
        }
    };

    // A functor that constructs `T`.
    template <typename T>
    static constexpr construct_t<T> construct{};


    enum class ToFunctorFlags
    {
        // For classes, return a helper callable class that store a refernce to yours.
        // This is useful if you're planning to inherit from it, but don't want copying.
        ref = 1 << 0,
    };
    EM_FLAG_ENUM(ToFunctorFlags)

    // This is what `ToFunctorFlags::ref` returns.
    template <reference F>
    struct FunctorRef
    {
        F ref;

        // Intentionally not perfect-forwarding `ref` based on the rvalue-ness of self.
        [[nodiscard]] constexpr auto operator()(auto &&... args) EM_RETURNS(ref(EM_FWD(args)...))
    };

    // Converts a function pointer or a class member pointer to a functor object. Existing functors are returned as is.
    // NOTE: This can't be optimal, as the resulting functors are stateful.
    // If your inputs are known at compile-time, prefer the macros from `em/macros/utils/lift.h`.
    template <ToFunctorFlags Flags = {}, Deduce..., typename T> requires std::is_class_v<std::remove_cvref_t<T>> && (!bool(Flags & ToFunctorFlags::ref))
    [[nodiscard]] constexpr auto ToFunctorObject(T &&func) noexcept -> T && {return EM_FWD(func);}
    template <ToFunctorFlags Flags = {}, Deduce..., typename T> requires std::is_class_v<std::remove_cvref_t<T>> && (bool(Flags & ToFunctorFlags::ref))
    [[nodiscard]] constexpr auto ToFunctorObject(T &&func) noexcept -> FunctorRef<T &&> {return {EM_FWD(func)};}
    template <ToFunctorFlags Flags = {}, Deduce..., typename T> requires std::is_function_v<std::remove_pointer_t<T>>
    [[nodiscard]] constexpr auto ToFunctorObject(T func) noexcept {return [func](auto &&... args) EM_RETURNS(func(EM_FWD(args)...));}
    template <ToFunctorFlags Flags = {}, Deduce..., typename T> requires std::is_member_pointer_v<T>
    [[nodiscard]] constexpr auto ToFunctorObject(T func) EM_RETURNS(EM_FWD(func))


    namespace detail::Functional
    {
        // Not using the base template, because we need different `requires` on both specializations,
        //   and adding one on the base template would force it onto the specialization too.
        template <typename F, typename A, typename B, typename ...C>
        struct FoldReturnType {};
        template <typename F, typename A, typename B> requires std::is_invocable_v<F &&, A &&, B &&>
        struct FoldReturnType<F, A, B> : std::invoke_result<F &&, A &&, B &&> {};
        template <typename F, typename A, typename B, typename C, typename ...D> requires std::is_invocable_v<F &&, A &&, typename FoldReturnType<F, B, C, D...>::type>
        struct FoldReturnType<F, A, B, C, D...> : std::invoke_result<F &&, A &&, typename FoldReturnType<F, B, C, D...>::type> {};

        // Here the base template handles `sizeof...(C) == 0`.
        template <typename F, typename A, typename B, typename ...C>
        struct FoldIsNoexcept : std::is_nothrow_invocable<F &&, A &&, B &&> {};
        template <typename F, typename A, typename B, typename C, typename ...D>
        struct FoldIsNoexcept<F, A, B, C, D...> : std::is_nothrow_invocable<F &&, A &&, typename FoldReturnType<F, B, C, D...>::type> {};
    }


    // Folds 2+ arguments over a binary function.
    // There's no overload for one argument because it's unclear if we should return by value or by reference in that case. Handle that manually.
    // This has to exist because you seemingly can't do recursive calls to free functions directly in `EM_RETURNS(...)`,
    //   because they don't work in return type `decltype(...)`, and in the `noexcept(...)` too. Lame.
    // Note that class member functions with deduced `this` can recurse normally without this helper.

    [[nodiscard]] EM_TINY constexpr auto Fold(auto &&func, auto &&a, auto &&b) EM_RETURNS(std::invoke(EM_FWD(func), EM_FWD(a), EM_FWD(b)))

    [[nodiscard]] EM_TINY constexpr auto Fold(auto &&func, auto &&a, auto &&b, auto &&c, auto &&... d)
        noexcept(detail::Functional::FoldIsNoexcept<decltype(func), decltype(a), decltype(b), decltype(c), decltype(d)...>::value)
        -> typename detail::Functional::FoldReturnType<decltype(func), decltype(a), decltype(b), decltype(c), decltype(d)...>::type
    {
        // Note, no forwarding the second use of `func` here.
        // Only the outermost call is allowed to forward it, which is handled by the two-argument overload above.

        // Also, we could use either `std::invoke` or `(Fold)` here. I don't see any difference, but directly `std::invoke` probably compiles faster.
        // So lets use that.
        return std::invoke(EM_FWD(func), EM_FWD(a), (Fold)(func, EM_FWD(b), EM_FWD(c), EM_FWD(d)...));
    }
}
