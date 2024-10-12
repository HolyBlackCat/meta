#pragma once

#include <type_traits>

namespace em::Meta
{
    // A tag. This can be used for ADL dispatch, which is intentional (`foo(tag<T>)` will search the namespace of `T` with ADL).
    template <typename T> struct tag {using type = T;};

    template <auto N> struct value_tag {static constexpr auto value = N;};
}
