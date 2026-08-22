#pragma once

#include "em/meta/common.h" // For `always_false` and `ValueTag`.
#include "em/meta/predicates.h"

#include <type_traits>
#include <utility>

// Type and value list manipulation.

namespace em::Meta::List
{
    namespace detail
    {
        template <typename T> struct from {};
        template <template <typename...> typename T, typename ...P> struct from<T<P...>> {using type = TypeList<P...>;};
        template <template <auto...> typename T, auto ...P> struct from<T<P...>> {using type = ValueList<P...>;};
    }

    // Generates a list from the arguments of an artibrary template.
    template <typename T> using from = typename detail::from<T>::type;


    namespace detail
    {
        template <template <typename...> typename T, typename L> struct apply_types {};
        template <template <typename...> typename T, typename ...P> struct apply_types<T, TypeList<P...>> {using type = T<P...>;};
        template <template <auto...> typename T, typename L> struct apply_values {};
        template <template <auto...> typename T, auto ...P> struct apply_values<T, ValueList<P...>> {using type = T<P...>;};
    }

    // Substitutes elements of a list into a type template.
    template <template <typename...> typename T, typename L> using apply_types = typename detail::apply_types<T, L>::type;
    template <template <auto...> typename T, typename L> using apply_values = typename detail::apply_values<T, L>::type;


    namespace detail
    {
        template <typename T> struct size {};
        template <typename ...P> struct size<TypeList<P...>> : std::integral_constant<std::size_t, sizeof...(P)> {};
        template <auto ...V> struct size<ValueList<V...>> : std::integral_constant<std::size_t, sizeof...(V)> {};
    }

    // Returns size of a `{type|value}_list`.
    template <typename T> constexpr std::size_t size = detail::size<T>::value;


    namespace detail
    {
        template <typename T, typename I = std::make_index_sequence<size<T>::value>> struct reverse {};
        template <typename ...T, std::size_t ...I> struct reverse<TypeList<T...>, std::index_sequence<I...>> {using type = TypeList<T...[sizeof...(T)-1-I]...>;};
        template <auto ...V, std::size_t ...I> struct reverse<ValueList<V...>, std::index_sequence<I...>> {using type = ValueList<V...[sizeof...(V)-1-I]...>;};
    }

    // Reverses a list.
    template <typename T> using reverse = typename detail::reverse<T>::type;


    namespace detail
    {
        template <typename T, typename ...P> struct append_types {};
        template <typename ...T, typename ...P> struct append_types<TypeList<T...>, P...> {using type = TypeList<T..., P...>;};
        template <typename T, auto ...P> struct append_values {};
        template <auto ...T, auto ...P> struct append_values<ValueList<T...>, P...> {using type = ValueList<T..., P...>;};
    }

    // Append zero or more elements to a list.
    template <typename T, typename ...P> using append_types = typename detail::append_types<T, P...>::type;
    template <typename T, auto ...P> using append_values = typename detail::append_values<T, P...>::type;


    namespace detail
    {
        // Concat lists.
        template <typename T, typename ...P> struct cat {};
        template <typename ...A> struct cat<TypeList<A...>> {using type = TypeList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <auto ...A> struct cat<ValueList<A...>> {using type = ValueList<A...>;}; // ^
        template <typename ...A, typename ...B> struct cat<TypeList<A...>, TypeList<B...>> {using type = TypeList<A..., B...>;};
        template <auto ...A, auto ...B> struct cat<ValueList<A...>, ValueList<B...>> {using type = ValueList<A..., B...>;};
        template <typename A, typename B, typename ...P> struct cat<A, B, P...> {using type = typename cat<A, typename cat<B, P...>::type>::type;};

        // Concat type lists (this allows passing zero lists and getting a `TypeList<>`).
        template <typename ...P> struct cat_types {};
        template <> struct cat_types<> {using type = TypeList<>;};
        template <typename ...A> struct cat_types<TypeList<A...>> {using type = TypeList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <typename ...A, typename ...B> struct cat_types<TypeList<A...>, TypeList<B...>> {using type = TypeList<A..., B...>;};
        template <typename A, typename ...P> struct cat_types<A, P...> {using type = typename cat_types<A, typename cat_types<P...>::type>::type;};

        // Concat value lists (this allows passing zero lists and getting a `ValueList<>`).
        template <typename ...P> struct cat_values {};
        template <> struct cat_values<> {using type = ValueList<>;};
        template <auto ...A> struct cat_values<ValueList<A...>> {using type = ValueList<A...>;}; // Using a parameter pack here to reject non-lists.
        template <auto ...A, auto ...B> struct cat_values<ValueList<A...>, ValueList<B...>> {using type = ValueList<A..., B...>;};
        template <typename A, typename ...P> struct cat_values<A, P...> {using type = typename cat_values<A, typename cat_values<P...>::type>::type;};
    }

    // Concatenates several lists.
    // `cat` can handle both types and values, but can't handle zero lists (sic: zero, not empty).
    // `cat_types` and `cat_values` only handle types and values respectively, and can handle zero lists (sic: zero, not empty).
    template <typename T, typename ...P> using cat = typename detail::cat<T, P...>::type;
    template <typename ...P> using cat_types = typename detail::cat_types<P...>::type;
    template <typename ...P> using cat_values = typename detail::cat_values<P...>::type;


    namespace detail
    {
        template <typename T> struct empty_of_same_kind {};
        template <typename ...P> struct empty_of_same_kind<TypeList<P...>> {using type = TypeList<>;};
        template <auto ...P> struct empty_of_same_kind<ValueList<P...>> {using type = ValueList<>;};
    }

    // Given a list, produces an empty `TypeList<>` or `ValueList<>`, matching the kind of the incoming list.
    template <typename T> using empty_of_same_kind = typename detail::empty_of_same_kind<T>::type;


    namespace detail
    {
        // Return ith element of a list.
        template <typename T, std::size_t I> struct at {};
        template <typename ...P, std::size_t I> struct at<TypeList<P...>, I> {using type = P...[I];};
        template <auto ...P, std::size_t I> struct at<ValueList<P...>, I> {static constexpr auto value = P...[I];};
    }

    // Returns ith element of a list.
    template <typename T, std::size_t I> using type_at = typename detail::at<T, I>::type;
    template <typename T, std::size_t I> constexpr auto value_at = detail::at<T, I>::value;


    namespace detail
    {
        template <typename L, typename T> struct contains_type {};
        template <typename ...P, typename T> struct contains_type<TypeList<P...>, T> : std::bool_constant<(std::is_same_v<P, T> || ...)> {};

        template <typename L, auto V> struct contains_value {};
        template <auto ...P, auto V> struct contains_value<ValueList<P...>, V> : std::bool_constant<((P == V) || ...)> {};
    }

    // Checks if a list contains an element, using a linear search.
    template <typename L, typename T> constexpr bool contains_type = detail::contains_type<L, T>::value;
    template <typename L, auto V> constexpr bool contains_value = detail::contains_value<L, V>::value;


    namespace detail
    {
        // Check if a list is a subset of another one.
        template <typename A, typename B> struct is_subset_of {};
        template <typename ...A, typename B> struct is_subset_of<TypeList<A...>, B> : std::bool_constant<(contains_type<B, A>::value && ...)> {};
        template <auto ...A, typename B> struct is_subset_of<ValueList<A...>, B> : std::bool_constant<(contains_value<B, A>::value && ...)> {};
    }

    // Check if a list is a subset of a different list (not strict).
    // I.e. each element of the first list is contained in the second list. So doesn't have to be a strict subset.
    // Works in quadratic time.
    template <typename A, typename B> constexpr bool is_subset_of = detail::is_subset_of<A, B>::value;


    namespace detail
    {
        // Check if both lists have the same elements, ignoring order and duplicates.
        template <typename A, typename B> struct have_same_elems : std::conjunction<is_subset_of<A, B>, is_subset_of<B, A>> {};
    }

    // Check if both lists have the same elements, ignoring order and duplicates.
    template <typename A, typename B> constexpr bool have_same_elems = detail::have_same_elems<A, B>::value;
    // Same, but also require list sizes to match.
    template <typename A, typename B> constexpr bool have_same_elems_and_size = have_same_elems<A, B> && size<A> == size<B>;


    namespace detail
    {
        template <typename L, typename T> struct find_type {};
        template <typename T>
        struct find_type<TypeList<>, T> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = false;
            using remaining = Meta::TypeList<>;
        };
        template <typename F, typename ...P, typename T>
        struct find_type<TypeList<F, P...>, T> : std::integral_constant<std::size_t, 1 + find_type<TypeList<P...>,T>::value>
        {
            static constexpr bool found = find_type::value <=/*sic*/ sizeof...(P);
            using remaining = typename find_type<TypeList<P...>,T>::remaining;
        };
        template <typename F, typename ...P>
        struct find_type<TypeList<F, P...>, F> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = true;
            using remaining = Meta::TypeList<P...>;
        };

        template <typename L, auto V> struct find_value {};
        template <auto V>
        struct find_value<ValueList<>, V> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = false;
            using remaining = Meta::ValueList<>;
        };
        template <auto F, auto ...P, auto V>
        struct find_value<ValueList<F, P...>, V> : std::integral_constant<std::size_t, 1 + find_value<ValueList<P...>,V>::value>
        {
            static constexpr bool found = find_value::value <=/*sic*/ sizeof...(P);
            using remaining = typename find_value<ValueList<P...>,V>::remaining;
        };
        template <auto F, auto ...P>
        struct find_value<ValueList<F, P...>, F> : std::integral_constant<std::size_t, 0>
        {
            static constexpr bool found = true;
            using remaining = Meta::ValueList<P...>;
        };
    }

    // Looks for an element in the list using linear search. Unlike `contains_type/contains_value`, provides additional information.
    // Those types have following members:
    // - `std::size_t value` - the element index, or the list size if not found.
    // - `bool found` - whether the element was found or not.
    // - `using remaining` - the list of elements after the one that was found, or an empty list if not found.
    template <typename L, typename T> using find_type = detail::find_type<L, T>;
    template <typename L, auto V> using find_value = detail::find_value<L, V>;


    namespace detail
    {
        template <typename A, typename B> struct copy_uniq {};
        template <typename B> struct copy_uniq<TypeList<>, B> {using type = B;};
        template <typename B> struct copy_uniq<ValueList<>, B> {using type = B;};
        template <typename A1, typename ...A, typename ...B> struct copy_uniq<TypeList<A1, A...>, TypeList<B...>>
        {
            using type = typename copy_uniq<TypeList<A...>, std::conditional_t<(std::is_same_v<A1, B> || ...), TypeList<B...>, TypeList<B..., A1>>>::type;
        };
        template <auto A1, auto ...A, auto ...B> struct copy_uniq<ValueList<A1, A...>, ValueList<B...>>
        {
            using type = typename copy_uniq<ValueList<A...>, std::conditional_t<((A1 == B) || ...), ValueList<B...>, ValueList<B..., A1>>>::type;
        };

        template <typename T> struct uniq {};
        template <typename ...P> struct uniq<TypeList<P...>> {using type = typename copy_uniq<TypeList<P...>, TypeList<>>::type;};
        template <auto ...P> struct uniq<ValueList<P...>> {using type = typename copy_uniq<ValueList<P...>, ValueList<>>::type;};
    }

    // Add all elements from `A` that don't exist in `B` to `B`.
    template <typename A, typename B> using copy_uniq = typename detail::copy_uniq<A, B>::type;
    // Remove duplicate elements from a list.
    template <typename T> using uniq = typename detail::uniq<T>::type;


    namespace detail
    {
        template <typename T, typename U, typename ...P> struct copy_subtract {};
        template <typename U, typename ...P> struct copy_subtract<TypeList<>, U, P...> {using type = U;};
        template <typename T0, typename ...T, typename ...U, typename ...P>
        struct copy_subtract<TypeList<T0, T...>, TypeList<U...>, P...> : copy_subtract<TypeList<T...>, TypeList<U...>, P...> {};
        template <typename T0, typename ...T, typename ...U, typename ...P> requires (!find_type<P, T0>::found && ...)
        struct copy_subtract<TypeList<T0, T...>, TypeList<U...>, P...> : copy_subtract<TypeList<T...>, TypeList<U..., T0>, P...> {};
        template <typename U, typename ...P> struct copy_subtract<ValueList<>, U, P...> {using type = U;};
        template <auto T0, auto ...T, auto ...U, typename ...P>
        struct copy_subtract<ValueList<T0, T...>, ValueList<U...>, P...> : copy_subtract<ValueList<T...>, ValueList<U...>, P...> {};
        template <auto T0, auto ...T, auto ...U, typename ...P> requires (!find_value<P, T0>::found && ...)
        struct copy_subtract<ValueList<T0, T...>, ValueList<U...>, P...> : copy_subtract<ValueList<T...>, ValueList<U..., T0>, P...> {};

        template <typename T, typename ...P> struct subtract {};
        template <typename ...T, typename ...P> struct subtract<TypeList<T...>, P...> : copy_subtract<TypeList<T...>, TypeList<>, P...> {};
        template <auto ...T, typename ...P> struct subtract<ValueList<T...>, P...> : copy_subtract<ValueList<T...>, ValueList<>, P...> {};
    }

    // Copies elements from list `T` to list `U`, but only those that don't appear in any of the lists `P...`.
    template <typename T, typename U, typename ...P> using copy_subtract = typename detail::copy_subtract<T, U, P...>::type;
    // Returns a list of elements from list `T` that don't appear in any of the lists `P...`.
    template <typename T, typename ...P> using subtract = typename detail::subtract<T, P...>::type;


    namespace detail
    {
        // Helpers for `subtract_ordered`. See the comments on the public typedef with that name below for an explanation.
        // Subtracts ordered lists from each other.

        // Searches for type/value `Sub` in list `In`. On success, appends the part of `In` before the element to `Out` and returns that as `::out`,
        //   and returns the remaining part of `In` without the element as `::in`.
        // If not found, returns `OrigOut` and `OrigIn` unchanged, as `::out` and `::in` respectively.
        // Here you should initially pass `OrigIn == In` and `OrigOut == Out`, or `::in` and `::out` from the previous iteration.
        template <typename OrigIn, typename OrigOut, typename In, typename Out, typename Sub> struct subtract_ordered_3_types {using in = OrigIn; using out = OrigOut;};
        template <typename OrigIn, typename OrigOut, typename Elem, typename ...In, typename Out              > struct subtract_ordered_3_types<OrigIn, OrigOut, TypeList<Elem, In...>, Out, Elem> {using in = TypeList<In...>; using out = Out;};
        template <typename OrigIn, typename OrigOut, typename Elem, typename ...In, typename Out, typename Sub> struct subtract_ordered_3_types<OrigIn, OrigOut, TypeList<Elem, In...>, Out, Sub > : subtract_ordered_3_types<OrigIn, OrigOut, TypeList<In...>, typename append_types<Out, Elem>::type, Sub> {};
        template <typename OrigIn, typename OrigOut, typename In, typename Out, auto Sub> struct subtract_ordered_3_values {using in = OrigIn; using out = OrigOut;};
        template <typename OrigIn, typename OrigOut, auto Elem, auto ...In, typename Out          > struct subtract_ordered_3_values<OrigIn, OrigOut, ValueList<Elem, In...>, Out, Elem> {using in = ValueList<In...>; using out = Out;};
        template <typename OrigIn, typename OrigOut, auto Elem, auto ...In, typename Out, auto Sub> struct subtract_ordered_3_values<OrigIn, OrigOut, ValueList<Elem, In...>, Out, Sub > : subtract_ordered_3_values<OrigIn, OrigOut, ValueList<In...>, typename append_values<Out, Elem>::type, Sub> {};

        // Applies `subtract_ordered_3` for each individual element in the list `Sub`, preserving `In` and `Out` between iterations.
        // Initially you should pass an empty list to `Out`.
        template <typename In, typename Out, typename Sub> struct subtract_ordered_2 {using type = typename cat<Out, In>::type;};
        template <typename In, typename Out, typename Sub0, typename ...Sub> struct subtract_ordered_2<In, Out, TypeList<Sub0, Sub...>> : subtract_ordered_2<typename subtract_ordered_3_types<In, Out, In, Out, Sub0>::in, typename subtract_ordered_3_types<In, Out, In, Out, Sub0>::out, TypeList<Sub...>> {};
        template <typename In, typename Out, auto Sub0, auto ...Sub> struct subtract_ordered_2<In, Out, ValueList<Sub0, Sub...>> : subtract_ordered_2<typename subtract_ordered_3_values<In, Out, In, Out, Sub0>::in, typename subtract_ordered_3_values<In, Out, In, Out, Sub0>::out, ValueList<Sub...>> {};

        // See the public `subtract_ordered` below for explanation.
        // Applies `subtract_ordered_2` for each list in `Sub...`.
        template <typename In, typename ...Sub> struct subtract_ordered {using type = In;};
        template <typename In, typename Sub0, typename ...Sub> struct subtract_ordered<In, Sub0, Sub...> : subtract_ordered<typename subtract_ordered_2<In, typename empty_of_same_kind<In>::type, Sub0>::type, Sub...> {};
    }

    // A supposedly more optimized version of `subtract` that only operates on ordered lists.
    // Each of the lists must be sorted in the same unspecified order. If they aren't sorted, some elements might not be removed.
    // It doesn't matter in what order the lists `P...` themselves are passed.
    // Internally does following:
    //   For each element to remove, scans the entire input list. If it finds that element, it will not rescan the part before it again,
    //   until we switch to the next list to subtract. Which means the order of lists `P...` doesn't matter, only the order of elements
    //   inside of each individual list matters.
    template <typename T, typename ...P>
    using subtract_ordered = typename detail::subtract_ordered<T, P...>::type;


    // The operations below depend on `predicates.h`:

    namespace detail
    {
        // These two implementations for types and values could have the same primary template, but that may or may not make compiler's job harder, so I'm keeping them separate.

        template <typename RetPass, typename RetFail, typename Pred, typename T>
        struct split_types {};

        template <typename RetPass, typename RetFail, typename Pred>
        struct split_types<RetPass, RetFail, Pred, TypeList<>> {using type = TypePair<RetPass, RetFail>;};
        template <typename ...RetPass, typename ...RetFail, typename Pred, typename P0, typename ...P> requires Pred::template type<P0>::value
        struct split_types<TypeList<RetPass...>, TypeList<RetFail...>, Pred, TypeList<P0, P...>> : split_types<TypeList<RetPass..., P0>, TypeList<RetFail...>, Pred, TypeList<P...>> {};
        template <typename ...RetPass, typename ...RetFail, typename Pred, typename P0, typename ...P> requires (!Pred::template type<P0>::value)
        struct split_types<TypeList<RetPass...>, TypeList<RetFail...>, Pred, TypeList<P0, P...>> : split_types<TypeList<RetPass...>, TypeList<RetFail..., P0>, Pred, TypeList<P...>> {};

        template <typename RetPass, typename RetFail, typename Pred, typename T>
        struct split_values {};

        template <typename RetPass, typename RetFail, typename Pred>
        struct split_values<RetPass, RetFail, Pred, ValueList<>> {using type = TypePair<RetPass, RetFail>;};
        template <auto ...RetPass, auto ...RetFail, typename Pred, auto P0, auto ...P> requires Pred::template type<P0>::value
        struct split_values<ValueList<RetPass...>, ValueList<RetFail...>, Pred, ValueList<P0, P...>> : split_values<ValueList<RetPass..., P0>, ValueList<RetFail...>, Pred, ValueList<P...>> {};
        template <auto ...RetPass, auto ...RetFail, typename Pred, auto P0, auto ...P> requires (!Pred::template type<P0>::value)
        struct split_values<ValueList<RetPass...>, ValueList<RetFail...>, Pred, ValueList<P0, P...>> : split_values<ValueList<RetPass...>, ValueList<RetFail..., P0>, Pred, ValueList<P...>> {};
    }

    // Given a predicate and a list, splits it into a `TypePair<A,B>` of two lists: first matching the predicate, and the second not matching.
    // The order and duplicates are preserved.
    template <TypePredicate Pred, typename T>
    using split_types = typename detail::split_types<TypeList<>, TypeList<>, Pred, T>::type;
    template <ValuePredicate Pred, typename T>
    using split_values = typename detail::split_values<ValueList<>, ValueList<>, Pred, T>::type;
}
