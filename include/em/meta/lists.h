#pragma once

#include "em/meta/constants.h" // For `always_false`.
#include "em/meta/tags.h" // For `ValueTag`.

#include <type_traits>
#include <utility>

// Type and value list manipulation.

namespace em::Meta
{
    template <typename...> struct TypeList {};
    template <auto...> struct ValueList {};


    namespace detail
    {
        // Generates a list from the arguments of an artibrary template.
        template <typename T> struct list_from {};
        template <template <typename...> typename T, typename ...P> struct list_from<T<P...>> {using type = TypeList<P...>;};
        template <template <auto...> typename T, auto ...P> struct list_from<T<P...>> {using type = ValueList<P...>;};

        // Substitute elements of a list into a template.
        template <template <typename...> typename T, typename L> struct list_apply_types {};
        template <template <typename...> typename T, typename ...P> struct list_apply_types<T, TypeList<P...>> {using type = T<P...>;};
        template <template <auto...> typename T, typename L> struct list_apply_values {};
        template <template <auto...> typename T, auto ...P> struct list_apply_values<T, ValueList<P...>> {using type = T<P...>;};

        // Returns size of a `TypeList` or `ValueList`.
        template <typename T> struct list_size {};
        template <typename ...P> struct list_size<TypeList<P...>> : std::integral_constant<std::size_t, sizeof...(P)> {};
        template <auto ...V> struct list_size<ValueList<V...>> : std::integral_constant<std::size_t, sizeof...(V)> {};

        // Reverses a list.
        // Here we use pack indexing. Porting to older compilers would need an alternative implementation.
        template <typename T, typename I = std::make_index_sequence<list_size<T>::value>> struct list_reverse {};
        template <typename ...T, std::size_t ...I> struct list_reverse<TypeList<T...>, std::index_sequence<I...>> {using type = TypeList<T...[sizeof...(T)-1-I]...>;};
        template <auto ...V, std::size_t ...I> struct list_reverse<ValueList<V...>, std::index_sequence<I...>> {using type = ValueList<V...[sizeof...(V)-1-I]...>;};

        // Append elements to a list.
        template <typename T, typename ...P> struct list_append_types {};
        template <typename ...T, typename ...P> struct list_append_types<TypeList<T...>, P...> {using type = TypeList<T..., P...>;};
        template <typename T, auto ...P> struct list_append_values {};
        template <auto ...T, auto ...P> struct list_append_values<ValueList<T...>, P...> {using type = ValueList<T..., P...>;};

        // Concat two lists.
        template <typename T, typename ...P> struct list_cat {};
        template <typename ...A> struct list_cat<TypeList<A...>> {using type = TypeList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <auto ...A> struct list_cat<ValueList<A...>> {using type = ValueList<A...>;}; // ^
        template <typename ...A, typename ...B> struct list_cat<TypeList<A...>, TypeList<B...>> {using type = TypeList<A..., B...>;};
        template <auto ...A, auto ...B> struct list_cat<ValueList<A...>, ValueList<B...>> {using type = ValueList<A..., B...>;};
        template <typename A, typename B, typename ...P> struct list_cat<A, B, P...> {using type = typename list_cat<A, typename list_cat<B, P...>::type>::type;};

        // Concat two type lists (this allows passing zero lists and getting a `TypeList<>`).
        template <typename ...P> struct list_cat_types {};
        template <> struct list_cat_types<> {using type = TypeList<>;};
        template <typename ...A> struct list_cat_types<TypeList<A...>> {using type = TypeList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <typename ...A, typename ...B> struct list_cat_types<TypeList<A...>, TypeList<B...>> {using type = TypeList<A..., B...>;};
        template <typename A, typename ...P> struct list_cat_types<A, P...> {using type = typename list_cat_types<A, typename list_cat_types<P...>::type>::type;};

        // Concat two value lists (this allows passing zero lists and getting a `ValueList<>`).
        template <typename ...P> struct list_cat_values {};
        template <> struct list_cat_values<> {using type = ValueList<>;};
        template <auto ...A> struct list_cat_values<ValueList<A...>> {using type = ValueList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <auto ...A, auto ...B> struct list_cat_values<ValueList<A...>, ValueList<B...>> {using type = ValueList<A..., B...>;};
        template <typename A, typename ...P> struct list_cat_values<A, P...> {using type = typename list_cat_values<A, typename list_cat_values<P...>::type>::type;};

        // Given a list, produces an empty `TypeList<>` or `ValueList<>`, matching the kind of the incoming list.
        template <typename T> struct list_empty_of_same_kind {};
        template <typename ...P> struct list_empty_of_same_kind<TypeList<P...>> {using type = TypeList<>;};
        template <auto ...P> struct list_empty_of_same_kind<ValueList<P...>> {using type = ValueList<>;};

        // Return ith element of a list.
        template <typename T, std::size_t I> struct list_at {};

        template <typename T, typename ...P, std::size_t I> struct list_at<TypeList<T, P...>, I>
        {
            using type = typename list_at<TypeList<P...>, I-1>::type;
        };
        template <typename T, typename ...P> struct list_at<TypeList<T, P...>, 0>
        {
            using type = T;
        };
        template <std::size_t I> struct list_at<TypeList<>, I>
        {
            static_assert(always_false<ValueTag<I>>, "List index is out of range.");
        };

        template <auto V, auto ...P, std::size_t I> struct list_at<ValueList<V, P...>, I>
        {
            static constexpr auto value = list_at<ValueList<P...>, I-1>::value;
        };
        template <auto V, auto ...P> struct list_at<ValueList<V, P...>, 0>
        {
            static constexpr auto value = V;
        };
        template <std::size_t I> struct list_at<ValueList<>, I>
        {
            static_assert(always_false<ValueTag<I>>, "List index is out of range.");
        };

        // Check if a list contains a value.
        template <typename L, typename T> struct list_contains_type {};
        template <typename ...P, typename T> struct list_contains_type<TypeList<P...>, T> : std::bool_constant<(std::is_same_v<P, T> || ...)> {};

        template <typename L, auto V> struct list_contains_value {};
        template <auto ...P, auto V> struct list_contains_value<ValueList<P...>, V> : std::bool_constant<((P == V) || ...)> {};

        // Check if a list is a subset of another one.
        template <typename A, typename B> struct list_is_subset_of {};
        template <typename ...A, typename B> struct list_is_subset_of<TypeList<A...>, B> : std::bool_constant<(list_contains_type<B, A>::value && ...)> {};
        template <auto ...A, typename B> struct list_is_subset_of<ValueList<A...>, B> : std::bool_constant<(list_contains_value<B, A>::value && ...)> {};

        // Check if both lists have the same elements, ignoring order and duplicates.
        template <typename A, typename B> struct lists_have_same_elems : std::conjunction<list_is_subset_of<A, B>, list_is_subset_of<B, A>> {};

        // Return index of the first occurence of an element in a list, or the size of the list if not found.
        template <typename L, typename T> struct list_find_type {};
        template <typename T>
        struct list_find_type<TypeList<>, T> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = false;
            using remaining = Meta::TypeList<>;
        };
        template <typename F, typename ...P, typename T>
        struct list_find_type<TypeList<F, P...>, T> : std::integral_constant<std::size_t, 1 + list_find_type<TypeList<P...>,T>::value>
        {
            static constexpr bool found = list_find_type::value <=/*sic*/ sizeof...(P);
            using remaining = typename list_find_type<TypeList<P...>,T>::remaining;
        };
        template <typename F, typename ...P>
        struct list_find_type<TypeList<F, P...>, F> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = true;
            using remaining = Meta::TypeList<P...>;
        };

        template <typename L, auto V> struct list_find_value {};
        template <auto V>
        struct list_find_value<ValueList<>, V> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = false;
            using remaining = Meta::ValueList<>;
        };
        template <auto F, auto ...P, auto V>
        struct list_find_value<ValueList<F, P...>, V> : std::integral_constant<std::size_t, 1 + list_find_value<ValueList<P...>,V>::value>
        {
            static constexpr bool found = list_find_value::value <=/*sic*/ sizeof...(P);
            using remaining = typename list_find_value<ValueList<P...>,V>::remaining;
        };
        template <auto F, auto ...P>
        struct list_find_value<ValueList<F, P...>, F> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = true;
            using remaining = Meta::ValueList<P...>;
        };

        // For each element of A, append it to B if it's not already in B.
        template <typename A, typename B> struct list_copy_uniq {};
        template <typename B> struct list_copy_uniq<TypeList<>, B> {using type = B;};
        template <typename B> struct list_copy_uniq<ValueList<>, B> {using type = B;};
        template <typename A1, typename ...A, typename ...B> struct list_copy_uniq<TypeList<A1, A...>, TypeList<B...>>
        {
            using type = typename list_copy_uniq<TypeList<A...>, std::conditional_t<(std::is_same_v<A1, B> || ...), TypeList<B...>, TypeList<B..., A1>>>::type;
        };
        template <auto A1, auto ...A, auto ...B> struct list_copy_uniq<ValueList<A1, A...>, ValueList<B...>>
        {
            using type = typename list_copy_uniq<ValueList<A...>, std::conditional_t<((A1 == B) || ...), ValueList<B...>, ValueList<B..., A1>>>::type;
        };

        // Remove duplicate elements from a list.
        template <typename T> struct list_uniq {};
        template <typename ...P> struct list_uniq<TypeList<P...>> {using type = typename list_copy_uniq<TypeList<P...>, TypeList<>>::type;};
        template <auto ...P> struct list_uniq<ValueList<P...>> {using type = typename list_copy_uniq<ValueList<P...>, ValueList<>>::type;};

        // Copy elements from `T` to `U` if they don't appear in any of the lists `P...`.
        template <typename T, typename U, typename ...P> struct list_copy_subtract {};
        template <typename U, typename ...P> struct list_copy_subtract<TypeList<>, U, P...> {using type = U;};
        template <typename T0, typename ...T, typename ...U, typename ...P>
        struct list_copy_subtract<TypeList<T0, T...>, TypeList<U...>, P...> : list_copy_subtract<TypeList<T...>, TypeList<U...>, P...> {};
        template <typename T0, typename ...T, typename ...U, typename ...P> requires (!list_find_type<P, T0>::found && ...)
        struct list_copy_subtract<TypeList<T0, T...>, TypeList<U...>, P...> : list_copy_subtract<TypeList<T...>, TypeList<U..., T0>, P...> {};
        template <typename U, typename ...P> struct list_copy_subtract<ValueList<>, U, P...> {using type = U;};
        template <auto T0, auto ...T, auto ...U, typename ...P>
        struct list_copy_subtract<ValueList<T0, T...>, ValueList<U...>, P...> : list_copy_subtract<ValueList<T...>, ValueList<U...>, P...> {};
        template <auto T0, auto ...T, auto ...U, typename ...P> requires (!list_find_value<P, T0>::found && ...)
        struct list_copy_subtract<ValueList<T0, T...>, ValueList<U...>, P...> : list_copy_subtract<ValueList<T...>, ValueList<U..., T0>, P...> {};

        // Get all elements in `T` that don't appear in any of the lists `P...`.
        template <typename T, typename ...P> struct list_subtract {};
        template <typename ...T, typename ...P> struct list_subtract<TypeList<T...>, P...> : list_copy_subtract<TypeList<T...>, TypeList<>, P...> {};
        template <auto ...T, typename ...P> struct list_subtract<ValueList<T...>, P...> : list_copy_subtract<ValueList<T...>, ValueList<>, P...> {};

        // Helpers for `list_subtract_ordered`. See the comments on the public typedef with that name below for an explanation.
        // Subtracts ordered lists from each other.

        // Searches for type/value `Sub` in list `In`. On success, appends the part of `In` before the element to `Out` and returns that as `::out`,
        //   and returns the remaining part of `In` without the element as `::in`.
        // If not found, returns `OrigOut` and `OrigIn` unchanged, as `::out` and `::in` respectively.
        // Here you should initially pass `OrigIn == In` and `OrigOut == Out`, or `::in` and `::out` from the previous iteration.
        template <typename OrigIn, typename OrigOut, typename In, typename Out, typename Sub> struct list_subtract_ordered_3_types {using in = OrigIn; using out = OrigOut;};
        template <typename OrigIn, typename OrigOut, typename Elem, typename ...In, typename Out              > struct list_subtract_ordered_3_types<OrigIn, OrigOut, TypeList<Elem, In...>, Out, Elem> {using in = TypeList<In...>; using out = Out;};
        template <typename OrigIn, typename OrigOut, typename Elem, typename ...In, typename Out, typename Sub> struct list_subtract_ordered_3_types<OrigIn, OrigOut, TypeList<Elem, In...>, Out, Sub > : list_subtract_ordered_3_types<OrigIn, OrigOut, TypeList<In...>, typename list_append_types<Out, Elem>::type, Sub> {};
        template <typename OrigIn, typename OrigOut, typename In, typename Out, auto Sub> struct list_subtract_ordered_3_values {using in = OrigIn; using out = OrigOut;};
        template <typename OrigIn, typename OrigOut, auto Elem, auto ...In, typename Out          > struct list_subtract_ordered_3_values<OrigIn, OrigOut, ValueList<Elem, In...>, Out, Elem> {using in = ValueList<In...>; using out = Out;};
        template <typename OrigIn, typename OrigOut, auto Elem, auto ...In, typename Out, auto Sub> struct list_subtract_ordered_3_values<OrigIn, OrigOut, ValueList<Elem, In...>, Out, Sub > : list_subtract_ordered_3_values<OrigIn, OrigOut, ValueList<In...>, typename list_append_values<Out, Elem>::type, Sub> {};

        // Applies `list_subtract_ordered_3` for each individual element in the list `Sub`, preserving `In` and `Out` between iterations.
        // Initially you should pass an empty list to `Out`.
        template <typename In, typename Out, typename Sub> struct list_subtract_ordered_2 {using type = typename list_cat<Out, In>::type;};
        template <typename In, typename Out, typename Sub0, typename ...Sub> struct list_subtract_ordered_2<In, Out, TypeList<Sub0, Sub...>> : list_subtract_ordered_2<typename list_subtract_ordered_3_types<In, Out, In, Out, Sub0>::in, typename list_subtract_ordered_3_types<In, Out, In, Out, Sub0>::out, TypeList<Sub...>> {};
        template <typename In, typename Out, auto Sub0, auto ...Sub> struct list_subtract_ordered_2<In, Out, ValueList<Sub0, Sub...>> : list_subtract_ordered_2<typename list_subtract_ordered_3_values<In, Out, In, Out, Sub0>::in, typename list_subtract_ordered_3_values<In, Out, In, Out, Sub0>::out, ValueList<Sub...>> {};

        // See the public `list_subtract_ordered` below for explanation.
        // Applies `list_subtract_ordered_2` for each list in `Sub...`.
        template <typename In, typename ...Sub> struct list_subtract_ordered {using type = In;};
        template <typename In, typename Sub0, typename ...Sub> struct list_subtract_ordered<In, Sub0, Sub...> : list_subtract_ordered<typename list_subtract_ordered_2<In, typename list_empty_of_same_kind<In>::type, Sub0>::type, Sub...> {};
    }

    // Generates a list from the arguments of an artibrary template.
    template <typename T> using list_from = typename detail::list_from<T>::type;

    // Substitutes elements of a list into a type template.
    template <template <typename...> typename T, typename L> using list_apply_types = typename detail::list_apply_types<T, L>::type;
    template <template <auto...> typename T, typename L> using list_apply_values = typename detail::list_apply_values<T, L>::type;

    // Returns size of a `{type|value}_list`.
    template <typename T> constexpr std::size_t list_size = detail::list_size<T>::value;

    // Reverses a list.
    template <typename T> using list_reverse = typename detail::list_reverse<T>::type;

    // Append elements to a list.
    template <typename T, typename ...P> using list_append_types = typename detail::list_append_types<T, P...>::type;
    template <typename T, auto ...P> using list_append_values = typename detail::list_append_values<T, P...>::type;

    // Concatenates several lists.
    // `list_cat` can handle both types and values, but can't handle zero lists.
    // `list_cat_types` and `list_cat_values` only handle types and values respectively, and can handle zero lists.
    template <typename T, typename ...P> using list_cat = typename detail::list_cat<T, P...>::type;
    template <typename ...P> using list_cat_types = typename detail::list_cat_types<P...>::type;
    template <typename ...P> using list_cat_values = typename detail::list_cat_values<P...>::type;

    // Given a list, produces an empty `TypeList<>` or `ValueList<>`, matching the kind of the incoming list.
    template <typename T> using list_empty_of_same_kind = typename detail::list_empty_of_same_kind<T>::type;

    // Return ith element of a list.
    template <typename T, std::size_t I> using list_type_at = typename detail::list_at<T, I>::type;
    template <typename T, std::size_t I> constexpr auto list_value_at = detail::list_at<T, I>::value;

    // Check if a list contains an element.
    template <typename L, typename T> constexpr bool list_contains_type = detail::list_contains_type<L, T>::value;
    template <typename L, auto V> constexpr bool list_contains_value = detail::list_contains_value<L, V>::value;

    // Check if a list is a subset of a different list.
    // I.e. each element of the first list is contained in the second list. So doesn't have to be a strict subset.
    template <typename A, typename B> constexpr bool list_is_subset_of = detail::list_is_subset_of<A, B>::value;

    // Check if both lists have the same elements, ignoring order and duplicates.
    template <typename A, typename B> constexpr bool lists_have_same_elems = detail::lists_have_same_elems<A, B>::value;
    // Same, but also require list sizes to match.
    template <typename A, typename B> constexpr bool lists_have_same_elems_and_size = lists_have_same_elems<A, B> && list_size<A> == list_size<B>;

    // Those types have following members:
    // - `std::size_t value` - the element index, or the list size if not found.
    // - `bool found` - whether the element was found or not.
    // - `using remaining` - the list of elements after the one that was found, or an empty list if not found.
    template <typename L, typename T> using list_find_type = detail::list_find_type<L, T>;
    template <typename L, auto V> using list_find_value = detail::list_find_value<L, V>;

    // Add all elements from `A` that don't exist in `B` to `B`.
    template <typename A, typename B> using list_copy_uniq = typename detail::list_copy_uniq<A, B>::type;
    // Remove duplicate elements from a list.
    template <typename T> using list_uniq = typename detail::list_uniq<T>::type;

    // Copies elements from list `T` to list `U`, but only those that don't appear in any of the lists `P...`.
    template <typename T, typename U, typename ...P> using list_copy_subtract = typename detail::list_copy_subtract<T, U, P...>::type;
    // Returns a list of elements from list `T` that don't appear in any of the lists `P...`.
    template <typename T, typename ...P> using list_subtract = typename detail::list_subtract<T, P...>::type;

    // A supposedly more optimized version of `list_subtract` that only operates on ordered lists.
    // Each of the lists must be sorted in the same unspecified order. If they aren't sorted, some elements might not be removed.
    // It doesn't matter in what order the lists `P...` themselves are passed.
    // Internally does following:
    //   For each element to remove, scans the entire input list. If it finds that element, it will not rescan the part before it again,
    //   until we switch to the next list to subtract. Which means the order of lists `P...` doesn't matter, only the order of elements
    //   inside of each individual list matters.
    template <typename T, typename ...P>
    using list_subtract_ordered = typename detail::list_subtract_ordered<T, P...>::type;
}
