#pragma once

#include "em/meta/common.h"

namespace em::Meta
{
    namespace detail::Predicates
    {
        template <template <typename> typename>
        struct CheckUnaryPredicate {};

        template <template <auto> typename>
        struct CheckUnaryValuePredicate {};
    }

    // Unary type predicate.
    // `T` needs to have a member template `type<T>`, with a constexpr `::value`.
    template <typename T>
    concept TypePredicate = requires{typename detail::Predicates::CheckUnaryPredicate<T::template type>;};

    // Unary value predicate.
    // `T` needs to have a member template `type<X>`, with a constexpr `::value`.
    template <typename T>
    concept ValuePredicate = requires{typename detail::Predicates::CheckUnaryValuePredicate<T::template type>;};

    // Given a type trait, returns a type matching `TypePredicate`.
    template <template <typename> typename T>
    struct TraitToTypePredicate
    {
        template <typename U>
        using type = T<U>;
    };

    // Given a value trait, returns a type matching `ValuePredicate`.
    template <template <auto> typename T>
    struct TraitToValuePredicate
    {
        template <auto U>
        using type = T<U>;
    };

    // Always true unary type predicate.
    using true_predicate = TraitToTypePredicate<true_trait>;
    // Always false unary type predicate.
    using false_predicate = TraitToTypePredicate<false_trait>;

    // Always true unary type predicate.
    using true_value_predicate = TraitToValuePredicate<true_value_trait>;
    // Always false unary type predicate.
    using false_value_predicate = TraitToValuePredicate<false_value_trait>;
}
