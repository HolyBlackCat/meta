#pragma once

#include "em/macros/utils/forward.h"
#include "em/meta/common.h"

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
    // If there are no arguments, consider using the `EM_VOID_TO_PLACEHOLDER(...)` macro instead of calling this directly.
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

// Same as `EM_RETURN_VARIABLE`, but also replaces `em::Meta::VoidPlaceholder` with `void`.
#define EM_RETURN_VARIABLE_OR_VOID(...) \
    DETAIL_EM_RETURN_VARIABLE( \
        (if constexpr (::std::is_same_v<::std::remove_cvref_t<decltype(__VA_ARGS__)>, ::em::Meta::VoidPlaceholder>) {return;} else), \
        __VA_ARGS__ \
    )

// If `...` is of type `void`, returns `em::Meta::VoidPlaceholder` by value.
// Otherwise returns it unchanged (works correctly even if it's a non-reference).
#define EM_VOID_TO_PLACEHOLDER(...) \
    ::em::Meta::InvokeWithVoidPlaceholderResult([&] -> decltype(auto) {return (__VA_ARGS__);}) // Intentionally return `(...)` to not copy variables.
