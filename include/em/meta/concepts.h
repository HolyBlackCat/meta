#pragma once

#include <concepts>
#include <type_traits>

namespace em::Meta
{
    template <typename T>
    concept bool_convertible = requires(const std::remove_cvref_t<T> &t){t ? true : false;};

    template <typename T>
    concept reference = std::is_reference_v<T>;


    namespace detail::Truthy
    {
        template <typename T>
        extern T declvar;
    }

    #ifdef __clang__
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wundefined-var-template"
    #endif

    // Returns true if the type `T` is always true when converted to `bool`, and is constexpr-convertible to bool even when it's not constexpr itself.
    // Such is e.g. `std::true_type` (but not references to it, at the time of writing).
    template <typename T>
    concept truthy_type = std::bool_constant<detail::Truthy::declvar<T> ? true : false>::value;

    // Returns true if the type `T` is always false when converted to `bool`, and is constexpr-convertible to bool even when it's not constexpr itself.
    // Such is e.g. `std::false_type` or `std::nullptr_t` (but not references to those, at the time of writing).
    template <typename T>
    concept falsey_type = std::bool_constant<detail::Truthy::declvar<T> ? false : true>::value;

    #ifdef __clang__
    #pragma clang diagnostic pop
    #endif
}
