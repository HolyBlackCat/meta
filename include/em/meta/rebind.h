#pragma once

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

    // If `T` has a member type `rebind<U>::other`, returns that type.
    // Otherwise if `T` is `TT<X, ...>`, returns `TT<U, ...>`.
    // Otherwise fails.
    template <typename T, typename U> requires requires{typename detail::Rebind<T, U>::type;}
    using rebind = typename detail::Rebind<T, U>::type;
}
