#pragma once

#include "em/meta/common.h"

namespace em::Meta
{
    namespace detail::TypePredicates
    {
        template <template <typename> typename>
        struct CheckUnaryPredicate {};
    }

    // Unary predicate.
    // `T` needs to have a member template `type<T>`, with a constexpr `::value`.
    template <typename T>
    concept TypePredicate = requires{typename detail::TypePredicates::CheckUnaryPredicate<T::template type>;};

    // Given a trait, returns a type matching `TypePredicate`.
    template <template <typename> typename T>
    struct TraitToPredicate
    {
        template <typename U>
        using type = T<U>;
    };

    // Always true unary type predicate.
    using true_predicate = TraitToPredicate<true_trait>;
    // Always false unary type predicate.
    using false_predicate = TraitToPredicate<false_trait>;
}
