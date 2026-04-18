#pragma once

#include "em/macros/utils/forward.h"
#include "em/meta/common.h"

#include <concepts>
#include <utility>

namespace em::Meta
{
    // Has a member variable `T value{};` that's reset when moved from.
    template <typename T>
    requires std::default_initializable<T> && std::movable<T>
    struct ResetMovedFromValue
    {
        T value{};

        [[nodiscard]] constexpr ResetMovedFromValue() {}
        [[nodiscard]] constexpr ResetMovedFromValue(const T &value) : value(value) {}
        [[nodiscard]] constexpr ResetMovedFromValue(T &&value) : value(std::move(value)) {}

        template <typename ...P> requires not_single_same_type_ignoring_cvref<ResetMovedFromValue, P...> && std::constructible_from<T, P &&...>
        [[nodiscard]] constexpr ResetMovedFromValue(P &&... params) : value(EM_FWD(params)...) {}

        constexpr ResetMovedFromValue(ResetMovedFromValue &&other) noexcept : value(std::move(other.value)) {other.value = T{};}
        constexpr ResetMovedFromValue &operator=(ResetMovedFromValue &&other) noexcept {value = std::move(other.value); other.value = T{}; return *this;}
    };

    // Inherits from `T`, which gets reset when moved from.
    template <typename T>
    requires std::default_initializable<T> && std::movable<T> && std::is_class_v<T> && (!std::is_final_v<T>)
    struct ResetMovedFromStruct : T
    {
        [[nodiscard]] constexpr ResetMovedFromStruct() : T() {}
        [[nodiscard]] constexpr ResetMovedFromStruct(const T &value) : T(value) {}
        [[nodiscard]] constexpr ResetMovedFromStruct(T &&value) : T(std::move(value)) {}

        template <typename ...P> requires not_single_same_type_ignoring_cvref<ResetMovedFromStruct, P...> && std::constructible_from<T, P &&...>
        [[nodiscard]] constexpr ResetMovedFromStruct(P &&... params) : T(EM_FWD(params)...) {}

        constexpr ResetMovedFromStruct(ResetMovedFromStruct &&other) noexcept : T(static_cast<T &&>(other)) {other.T::operator=(T{});}
        constexpr ResetMovedFromStruct &operator=(ResetMovedFromStruct &&other) noexcept {T::operator=(static_cast<T &&>(other)); other.T::operator=(T{}); return *this;}
    };
}
