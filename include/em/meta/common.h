#pragma once

#include <concepts>
#include <type_traits>

namespace em::Meta
{
    // --- Tags.

    // A tag. This can be used for ADL dispatch, which is intentional (`foo(Tag<T>{})` will search the namespace of `T` with ADL).
    template <typename T> struct Tag {using type = T;};

    template <auto N> struct ValueTag {static constexpr auto value = N;};


    // --- Force template argument deduction.

    // This can be used as `template <__, Deduce..., __>`, to force the template parameters after it to be deduced.
    // Don't rely on this being a concept, only use the exact syntax given above.
    template <typename> concept Deduce = false;


    // --- Some basic concepts.

    template <typename T>
    concept bool_convertible = requires(const std::remove_cvref_t<T> &t){t ? true : false;};

    template <typename T>
    concept reference = std::is_reference_v<T>;


    // --- Check lack of qualifiers.

    template <typename T> concept cv_unqualified = std::is_same_v<std::remove_cv_t<T>, T>;
    template <typename T> concept cvref_unqualified = std::is_same_v<std::remove_cvref_t<T>, T>;


    // --- Comparison ignoring qualifiers.

    template <typename A, typename B> concept same_ignoring_cv = std::same_as<std::remove_cv_t<A>, std::remove_cv_t<B>>;
    template <typename A, typename B> concept same_ignoring_cvref = std::same_as<std::remove_cvref_t<A>, std::remove_cvref_t<B>>;


    // --- Inheritance and qualifiers.

    // Like `std::derived_from`, except it permits the same non-class type, and always returns if there are any cvref-qualifiers on either type.
    template <typename Derived, typename Base>
    concept same_or_derived_from = cvref_unqualified<Derived> && cvref_unqualified<Base> && (std::same_as<Derived, Base> || std::derived_from<Derived, Base>);

    // Like `std::derived_from`, but ignores cvref. And also returns true for same non-class types, while `std::derived_from` doesn't.
    template <typename Derived, typename Base>
    concept same_or_derived_from_ignoring_cvref = same_or_derived_from<std::remove_cvref_t<Derived>, std::remove_cv_t<Base>>;


    // `Derived` is derived from `Base` (or are the same type, possibly non-class) (ignoring cvref-qualifiers on both),
    // and `Derived` is convertible to `Base` (respecting the cvref-qualifiers.
    template <typename Derived, typename Base>
    concept same_or_derived_from_and_cvref_convertible_to =
        (same_ignoring_cvref<Derived, Base> || std::derived_from<std::remove_cvref_t<Derived>, std::remove_cvref_t<Base>>) &&
        std::convertible_to<Derived &&, Base>; // Note the added `&&` on the first argument. Not doing that explodes on incomplete types for no good reason.


    // --- Add qualifiers.

    namespace detail
    {
        template <typename T> struct const_type {using type = const T;};
        template <typename T> struct const_type<T &> {using type = const T &;};
        template <typename T> struct const_type<T &&> {using type = const T &&;};
    }

    // Exactly like `const T` and `std::add_const_t`, has no effect on references.
    template <typename T> using const_type = const T;

    // Like `const T`, but when applied to a reference, modifies the referenced type.
    template <typename T> using const_type_cvref = typename detail::const_type<T>::type;

    // Forwards `x` with constness added. Unlike `std::as_const()`, this can also handle rvalues.
    inline constexpr auto make_const = []<typename T> [[nodiscard]] (T &&x) -> auto &&
    {
        return static_cast<const_type<T &&>>(x);
    };


    // --- Conditional constness.

    namespace detail
    {
        template <bool Const, typename T> struct maybe_const {using type = T;};
        template <typename T> struct maybe_const<true, T> {using type = const T;};

        template <bool Const, typename T> struct maybe_const_cvref {using type = T;};
        template <typename T> struct maybe_const_cvref<true, T> {using type = typename const_type<T>::type;};
    }

    // If `Const`, applies `const_type` to `T`, otherwise leaves it unchanged.
    template <bool Const, typename T>
    using maybe_const = typename detail::maybe_const<Const, T>::type;

    // If `Const`, applies `const_type_cvref` to `T`, otherwise leaves it unchanged.
    template <bool Const, typename T>
    using maybe_const_cvref = typename detail::maybe_const_cvref<Const, T>::type;


    // --- Copy qualifiers from one type to another.

    namespace detail
    {
        template <typename A, typename B> struct copy_cv_impl                      {using type =                B;};
        template <typename A, typename B> struct copy_cv_impl<const          A, B> {using type = const          B;};
        template <typename A, typename B> struct copy_cv_impl<      volatile A, B> {using type =       volatile B;};
        template <typename A, typename B> struct copy_cv_impl<const volatile A, B> {using type = const volatile B;};

        template <typename A, typename B> struct copy_cvref_impl          {using type = typename copy_cv_impl<A, B>::type;};
        template <typename A, typename B> struct copy_cvref_impl<A & , B> {using type = typename copy_cv_impl<A, B>::type &;};
        template <typename A, typename B> struct copy_cvref_impl<A &&, B> {using type = typename copy_cv_impl<A, B>::type &&;};
    }

    // Ignores referenceness on `A`, adds its cv-qualifiers to `B`. Naturally has no effect if `B` is a reference.
    template <typename A, typename B> using copy_cv    = typename detail::copy_cv_impl<std::remove_reference_t<A>, B>::type;
    // Adds cvref-qualifiers from `A` to `B`. If `B` is a reference, the cv-qualifiers of `A` are ignored and the normal reference collapsing rules apply.
    template <typename A, typename B> using copy_cvref = typename detail::copy_cvref_impl<A, B>::type;


    // --- Some constants.

    template <typename, typename...>
    inline constexpr auto always_false = false;

    template <typename, typename...> struct true_trait : std::true_type {};
    template <typename, typename...> struct false_trait : std::false_type {};


    // --- Basic pack manipulation.

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


    // --- Specializations.

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
