#pragma once

#include "em/macros/portable/tiny_func.h"
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


    // See `construct` below.
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

    // If `T` isn't an lvalue reference, returns `value` with the forwarding preserved.
    // Otherwise forces it to be an lvalue reference.
    template <typename T, Deduce..., typename U>
    [[nodiscard]] constexpr auto &&combine_forwarding(U &&value) noexcept
    {
        if constexpr (std::is_lvalue_reference_v<T>)
            return static_cast<U &>(value);
        else
            return EM_FWD(value);
    }


    // A functor that acts as a reference to another functor.
    template <reference F>
    struct FuncRef
    {
        F ref;

        // Writing this in a very particular way, to forward the functor only if both `ref` and `self` are rvalue references.
        [[nodiscard]] constexpr auto operator()(this auto &&self, auto &&... args) EM_RETURNS(std::invoke((combine_forwarding<decltype(self)>)(EM_FWD(self.ref)), EM_FWD(args)...))
    };
    template <typename F> requires(!specialization_of_ignoring_cvref<F, FuncRef>)
    FuncRef(F &&) -> FuncRef<F &&>;
    template <typename T>
    FuncRef(const FuncRef<T> &) -> FuncRef<T>; // This handles all other cvref variants of the argument too. Can't pass by value here, because it might not be copyable,

    // Wraps the argument in `FuncRef` and then calls `std::not_fn()` on it to negate it.
    [[nodiscard]] constexpr auto MakeNegatedFuncRef(auto &&func) EM_RETURNS(std::not_fn(FuncRef(EM_FWD(func))))


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
