#pragma once

#include <type_traits>

namespace em::Meta
{
    template <typename, typename...>
    inline constexpr auto always_false = false;

    template <typename, typename...> struct true_trait : std::true_type {};
    template <typename, typename...> struct false_trait : std::false_type {};
}
