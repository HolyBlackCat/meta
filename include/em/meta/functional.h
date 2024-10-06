#pragma once

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
}
