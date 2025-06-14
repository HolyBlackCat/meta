#pragma once

#include "em/meta/common.h"

// Generalizes `std::allocator_traits::rebind` for arbitrary types.

namespace em::Meta
{
    namespace detail
    {
        template <typename T, typename U>
        concept HasMemberRebind = requires{typename T::template rebind<U>::other;};

        template <typename T, typename U>
        struct Rebind {};

        template <typename T, typename U> requires HasMemberRebind<T, U>
        struct Rebind<T, U> {using type = typename T::template rebind<U>::other;};

        template <template <typename...> typename T, typename P0, typename ...P, typename U> requires (!HasMemberRebind<T<P0, P...>, U>)
        struct Rebind<T<P0, P...>, U> {using type = T<U, P...>;};
    }

    // If `P` has a member type `rebind<T>::other`, returns that type.
    // Otherwise if `P` is `TT<X, ...>`, returns `TT<U, ...>`. Otherwise fails.
    // Repeats this for every `P...`, and fails if the results are different.
    template <typename T, typename ...P>
    using rebind_to = require_same_type<typename detail::Rebind<P, T>::type...>;
}
