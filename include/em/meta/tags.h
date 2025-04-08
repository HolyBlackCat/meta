#pragma once

namespace em::Meta
{
    // A tag. This can be used for ADL dispatch, which is intentional (`foo(Tag<T>{})` will search the namespace of `T` with ADL).
    template <typename T> struct Tag {using type = T;};

    template <auto N> struct ValueTag {static constexpr auto value = N;};
}
