#pragma once

#include <concepts>

// Basic operations on packs of types.

namespace em::Meta
{
    template <typename, typename...>
    inline constexpr auto always_false = false;


    namespace detail
    {
        // Need this class to avoid `Pack expansion used as argument for non-pack parameter of alias template`.
        // Also for `void_type` to not be buggy.
        template <typename T, typename ...P> struct FirstType {using type = T;};
    }

    // Returns the first type in the pack.
    template <typename ...P>
    using first_type = typename detail::FirstType<P...>::type;

    // Always `void`.
    template <typename ...P>
    using void_type = first_type<void, P...>;


    // Checks if all types are equal.
    // Need `first_type` to avoid `Pack expansion used as argument for non-pack parameter of alias template` in some usages.
    template <typename ...P> concept same_as_all = (std::same_as<first_type<P...>, P> && ...);

    // Checks if the first type is the same as any of the remaining ones.
    template <typename T, typename ...P> concept same_as_any = (std::same_as<T, P> || ...);

    // If all template parameters are the same type, returns that type. Otherwise fails.
    template <typename ...P> requires same_as_all<P...>
    using require_same_type = first_type<P...>;
}
