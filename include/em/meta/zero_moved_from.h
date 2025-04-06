#pragma once

#include "em/meta/cvref.h"

#include <type_traits>
#include <utility>

namespace em::Meta
{
    // A value wrapper that zeroes the moved-from value.
    // This is copyable if `T` is copyable.
    // Note that this doesn't replace the copy&swap idiom.
    template <typename T>
    struct ZeroMovedFrom
    {
        T value{};

        constexpr ZeroMovedFrom() {}

        template <typename P0, typename ...P>
        requires
            (sizeof...(P) > 0 || !derived_from_ignoring_cvref<P0, ZeroMovedFrom>) && // Avoid conflicts with copy-move constructors.
            std::is_constructible_v<T, P0 &&, P &&...>
            constexpr ZeroMovedFrom(P0 &&params0, P &&... params) noexcept(std::is_nothrow_constructible_v<T, P0 &&, P &&...>)
            : value(std::forward<P0>(params0), std::forward<P>(params)...)
        {}

        constexpr ZeroMovedFrom(const ZeroMovedFrom &other) noexcept(std::is_nothrow_copy_constructible_v<T>) requires std::is_copy_constructible_v<T> = default;
        constexpr ZeroMovedFrom &operator=(const ZeroMovedFrom &other) noexcept(std::is_nothrow_copy_assignable_v<T>) requires std::is_copy_assignable_v<T> = default;

        constexpr ZeroMovedFrom(ZeroMovedFrom &&other) noexcept(std::is_nothrow_move_constructible_v<T>) requires std::is_move_constructible_v<T>
            : value(std::move(other.value))
        {
            other.value = T{};
        }

        constexpr ZeroMovedFrom &operator=(ZeroMovedFrom &&other) noexcept(std::is_nothrow_move_assignable_v<T>) requires std::is_move_assignable_v<T>
        {
            value = std::move(other.value);
            other.value = {};
            return *this;
        }
    };
}
