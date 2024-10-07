#pragma once

#include <type_traits>

namespace em::Meta
{
    template <typename T> struct tag {using type = T;};
    template <auto N> struct value_tag {static constexpr auto value = N;};
}
