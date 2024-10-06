#pragma once

#include <concepts>
#include <type_traits>

// Manipulating cvref-qualifiers.

namespace em::Meta
{
    // Some concepts.

    template <typename T> concept cv_unqualified = std::is_same_v<std::remove_cv_t<T>, T>;
    template <typename T> concept cvref_unqualified = std::is_same_v<std::remove_cvref_t<T>, T>;


    // Conditional constness.
    // `maybe_const<true, T>` is `const T`, and `maybe_const<false, T>` is just `T`.

    namespace detail
    {
        template <bool Const, typename T>
        struct maybe_const {using type = T;};

        template <typename T>
        struct maybe_const<true, T> {using type = const T;};
    }

    template <bool Const, typename T>
    using maybe_const = typename detail::maybe_const<Const, T>::type;


    // Copy qualifiers from one type to another.
    // Given `cvA refA A` and `cvB refB B`:
    // * `copy_cv`    returns `cvA refB B`
    // * `copy_cvref` returns `cvA refA B`

    namespace detail
    {
        template <typename A, typename B> struct copy_cv_low                      {using type =                B;};
        template <typename A, typename B> struct copy_cv_low<const          A, B> {using type = const          B;};
        template <typename A, typename B> struct copy_cv_low<      volatile A, B> {using type =       volatile B;};
        template <typename A, typename B> struct copy_cv_low<const volatile A, B> {using type = const volatile B;};

        template <typename A, typename B> struct copy_cv          {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type;};
        template <typename A, typename B> struct copy_cv<A, B & > {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type &;};
        template <typename A, typename B> struct copy_cv<A, B &&> {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type &&;};

        template <typename A, typename B> struct copy_cvref          {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type;};
        template <typename A, typename B> struct copy_cvref<A & , B> {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type &;};
        template <typename A, typename B> struct copy_cvref<A &&, B> {using type = typename copy_cv_low<A, std::remove_cvref_t<B>>::type &&;};
    }

    template <typename A, typename B> using copy_cv = typename detail::copy_cv<std::remove_reference_t<A>, B>::type;
    template <typename A, typename B> using copy_cvref = typename detail::copy_cvref<A, B>::type;


    // Comparison ignoring qualifiers.

    template <typename A, typename B> concept same_ignoring_cv = std::same_as<std::remove_cv_t<A>, std::remove_cv_t<B>>;
    template <typename A, typename B> concept same_ignoring_cvref = std::same_as<std::remove_cvref_t<A>, std::remove_cvref_t<B>>;
}
