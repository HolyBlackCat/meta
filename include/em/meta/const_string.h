#pragma once

#include "em/zstring_view.h"

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>

namespace em::Meta
{
    // A string that can be used as a template parameter.
    // `N` includes the null-terminator.
    template <std::size_t N>
    struct ConstString
    {
        char str[N]{};

        static constexpr std::size_t size = N - 1;

        consteval ConstString() {}
        consteval ConstString(const char (&new_str)[N])
        {
            if (new_str[N-1] != '\0')
                std::unreachable();
            std::copy_n(new_str, size, str);
        }

        [[nodiscard]] constexpr zstring_view view() const &
        {
            return zstring_view(zstring_view::TrustSpecifiedSize{}, std::string_view(str, str + size));
        }
        [[nodiscard]] constexpr zstring_view view() const && = delete;
    };

    template <std::size_t A, std::size_t B>
    [[nodiscard]] constexpr ConstString<A + B - 1> operator+(const ConstString<A> &a, const ConstString<B> &b)
    {
        ConstString<A + B - 1> ret;
        std::copy_n(a.str, a.size, ret.str);
        std::copy_n(b.str, b.size, ret.str + a.size);
        return ret;
    }

    template <std::size_t A, std::size_t B>
    [[nodiscard]] constexpr ConstString<A + B - 1> operator+(const ConstString<A> &a, const char (&b)[B])
    {
        return a + ConstString<B>(b);
    }

    template <std::size_t A, std::size_t B>
    [[nodiscard]] constexpr ConstString<A + B - 1> operator+(const char (&a)[A], const ConstString<B> &b)
    {
        return ConstString<A>(a) + b;
    }


    // A tag structure returned by `operator""_const` below.
    template <Meta::ConstString S>
    struct ConstStringTag
    {
        static constexpr Meta::ConstString value = S;
    };

    // Returns a string encoded into a template parameter of a tag structure `ConstStringTag`.
    template <Meta::ConstString S>
    [[nodiscard]] constexpr ConstStringTag<S> operator""_c()
    {
        return {};
    }
}

namespace em::inline Common
{
    using em::Meta::operator""_c;
}
