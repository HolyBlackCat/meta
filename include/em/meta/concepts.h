#pragma once

#include <concepts>
#include <type_traits>

namespace em::Meta
{
    template <typename T>
    concept bool_convertible = requires(const std::remove_cvref_t<T> &t){t ? true : false;};

    template <typename T>
    concept reference = std::is_reference_v<T>;

    template <typename T>
    concept truthy_type = std::derived_from<std::remove_cvref_t<T>, std::true_type> && !std::derived_from<std::remove_cvref_t<T>, std::false_type>;

    template <typename T>
    concept falsey_type = std::derived_from<std::remove_cvref_t<T>, std::false_type> && !std::derived_from<std::remove_cvref_t<T>, std::true_type>;
}
