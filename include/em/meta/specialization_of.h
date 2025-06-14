#pragma once

#include "em/meta/common.h"

#include <type_traits>

namespace em::Meta
{
    namespace detail
    {
        template <typename T, template <typename...> typename U> struct SpecializationOf : std::false_type {};
        template <typename ...P, template <typename...> typename U> struct SpecializationOf<U<P...>, U> : std::true_type {};
    }

    // Returns true if `T` is a `U<...>`, ignoring cvref-qualifiers.
    // This only works if `U` only has type template parameters. It appears to be impossible to support arbitrary types.
    template <typename T, template <typename...> typename U>
    concept specialization_of_ignoring_cvref = detail::SpecializationOf<std::remove_cvref_t<T>, U>::value;

    // Returns true if `T` is a `U<...>` (cvref-unqualified).
    // This only works if `U` only has type template parameters. It appears to be impossible to support arbitrary types.
    template <typename T, template <typename...> typename U>
    concept specialization_of = cvref_unqualified<T> && specialization_of_ignoring_cvref<T, U>;
}
