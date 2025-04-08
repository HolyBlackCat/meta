#pragma once

#include "em/macros/portable/tiny_func.h"
#include "em/macros/utils/forward.h"
#include "em/macros/utils/returns.h"

#include <functional>
#include <type_traits>

namespace em::Meta
{
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
    // This has to exist because you can't do recursive calls directly in `EM_RETURNS(...)`, because they don't work in return type `decltype(...)`,
    //   and in `noexcept(...)` too. Lame.

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
