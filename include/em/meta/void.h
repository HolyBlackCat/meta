#pragma once

#include "em/meta/deduce.h"
#include "em/meta/cvref.h"

#include <functional>
#include <utility>

// Helps deal with the `void` type.

namespace em::Meta
{
    // A placeholder for the `void` type.
    struct VoidPlaceholder {};

    // Checks if `T` is `VoidPlaceholder`, maybe cvref-qualified.
    template <typename T>
    concept IsVoidPlaceholder = same_ignoring_cvref<T, VoidPlaceholder>;

    // Invokes `func(args...)`. Forwards the return value, except if it returns void, replaces that with `VoidPlaceholder`.
    template <Deduce..., typename ...P, std::invocable<P...> F>
    [[nodiscard]] constexpr decltype(auto) InvokeWithVoidPlaceholderResult(F &&func, P &&... args)
    {
        if constexpr (std::is_void_v<std::invoke_result_t<F &&, P &&...>>)
        {
            std::invoke(std::forward<F>(func), std::forward<P>(args)...);
            return VoidPlaceholder{};
        }
        else
        {
            return std::invoke(std::forward<F>(func), std::forward<P>(args)...);
        }
    }

    // If `cond` is the `VoidPlaceholder`, returns `Value` by value. Otherwise forwards `cond` and returns a reference.
    template <auto Value, Deduce..., typename T>
    [[nodiscard]] constexpr decltype(auto) VoidTo(T &&cond)
    {
        if constexpr (IsVoidPlaceholder<T>)
            return Value;
        else
            return std::forward<T>(cond);
    }
}
