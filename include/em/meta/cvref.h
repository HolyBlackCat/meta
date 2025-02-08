#pragma once

#include "em/macros/utils/implies.h"
#include "em/macros/utils/flag_enum.h"

#include <concepts>
#include <type_traits>
#include <typeinfo>

// Manipulating cvref-qualifiers.

namespace em::Meta
{
    // --- Check lack of qualifiers.

    template <typename T> concept cv_unqualified = std::is_same_v<std::remove_cv_t<T>, T>;
    template <typename T> concept cvref_unqualified = std::is_same_v<std::remove_cvref_t<T>, T>;


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
    // Adds cvref-qualifiers from `A` to `B`. If `B` is a reference, cv-qualifiers are ignored and the normal reference collapsing rules apply.
    template <typename A, typename B> using copy_cvref = typename detail::copy_cvref_impl<A, B>::type;


    // Comparison ignoring qualifiers.

    template <typename A, typename B> concept same_ignoring_cv = std::same_as<std::remove_cv_t<A>, std::remove_cv_t<B>>;
    template <typename A, typename B> concept same_ignoring_cvref = std::same_as<std::remove_cvref_t<A>, std::remove_cvref_t<B>>;


    // Representing cvref as flags.

    enum class CvrefFlags : unsigned char
    {
        none = 0,
        const_ = 0b001,
        lvalue_ref = 0b010,
        rvalue_ref = 0b100,
        const_lvalue_ref = const_ | lvalue_ref,
        const_rvalue_ref = const_ | rvalue_ref,

        // Not a valid value, use this mask to extract reference-ness.
        ref_mask = lvalue_ref | rvalue_ref,

        // All valid values are less then than this.
        end_value = 0b110,
    };
    EM_FLAG_ENUM(CvrefFlags)

    namespace detail
    {
        template <typename T> struct QualToFlags             : std::integral_constant<CvrefFlags, CvrefFlags::none> {};
        template <typename T> struct QualToFlags<const T   > : std::integral_constant<CvrefFlags, CvrefFlags::const_> {};
        template <typename T> struct QualToFlags<      T & > : std::integral_constant<CvrefFlags, CvrefFlags::lvalue_ref> {};
        template <typename T> struct QualToFlags<const T & > : std::integral_constant<CvrefFlags, CvrefFlags::const_lvalue_ref> {};
        template <typename T> struct QualToFlags<      T &&> : std::integral_constant<CvrefFlags, CvrefFlags::rvalue_ref> {};
        template <typename T> struct QualToFlags<const T &&> : std::integral_constant<CvrefFlags, CvrefFlags::const_rvalue_ref> {};

        template <CvrefFlags F, typename T> struct FlagsToQual {};
        template <typename T> struct FlagsToQual<CvrefFlags::none            , T> {using type =       T;};
        template <typename T> struct FlagsToQual<CvrefFlags::const_          , T> {using type = const T;};
        template <typename T> struct FlagsToQual<CvrefFlags::lvalue_ref      , T> {using type =       T &;};
        template <typename T> struct FlagsToQual<CvrefFlags::const_lvalue_ref, T> {using type = const T &;};
        template <typename T> struct FlagsToQual<CvrefFlags::rvalue_ref      , T> {using type =       T &&;};
        template <typename T> struct FlagsToQual<CvrefFlags::const_rvalue_ref, T> {using type = const T &&;};
    }

    // Extracts cvref-qualifiers from a type to flags.
    template <typename T> constexpr CvrefFlags cvref_to_flags = detail::QualToFlags<T>::value;

    // Adds cvref-qualifiers from flags to a type. If the target is a reference, constness is ignored, and the normal reference collapsing rules apply.
    template <CvrefFlags F, typename T> using flags_to_cvref = typename detail::FlagsToQual<F, T>::type;


    // Combines `CvrefFlags` and `std::type_info`.
    struct CvrefFlagsAndType
    {
        CvrefFlags flags{};

        // This should be `std::type_index` by value, but that currently doesn't have a constexpr `operator==`.
        const std::type_info *type = nullptr;
    };

    // Returns `CvrefFlags` and `std::type_info` for a type.
    template <typename T>
    constexpr CvrefFlagsAndType type_to_desc = {.flags = cvref_to_flags<T>, .type = &typeid(T)};


    // Checks that `from` flags are convertible to `to` flags.
    // If `to` is a non-ref, this version always returns false (you must check for that manually, taking into account copyability/movability of the type).
    // If `from` is a non-ref, it's assumed to be an rvalue ref. You can adjust it manually before calling if you don't like this.
    [[nodiscard]] constexpr bool CvrefFlagsConvertible(CvrefFlags from, CvrefFlags to)
    {
        if (!bool(to & CvrefFlags::ref_mask))
            return false; // See comment above.
        if (to == CvrefFlags::const_lvalue_ref)
            return true;
        return
            // `from` const implies `to` const
            (bool(from & CvrefFlags::const_) EM_IMPLIES bool(to & CvrefFlags::const_)) &&
            // If `from` is rvalue ref or non-ref, `to` must be rvalue ref (non-ref was rejected before). Or both must be lvalue refs.
            ((from & CvrefFlags::ref_mask) == CvrefFlags::lvalue_ref) == ((to & CvrefFlags::ref_mask) == CvrefFlags::lvalue_ref);
    }

    // Checks that `desc` describes the same type as `desc` and the cvref-qualifiers are compatible.
    // If `from` is a non-ref, it's assumed to be an rvalue ref. You can adjust it manually before calling if you don't like this.
    template <typename T>
    [[nodiscard]] constexpr bool SameTypeAndConstructibleFromDesc(const CvrefFlagsAndType &desc)
    {
        if (*desc.type != typeid(T))
            return false;

        if constexpr (std::is_reference_v<T>)
        {
            return CvrefFlagsConvertible(desc.flags, cvref_to_flags<T>);
        }
        else
        {
            if (bool(desc.flags & CvrefFlags::const_))
                return (desc.flags & CvrefFlags::ref_mask) == CvrefFlags::lvalue_ref ? std::is_constructible_v<T, const T &> : std::is_constructible_v<T, const T &&>;
            else
                return (desc.flags & CvrefFlags::ref_mask) == CvrefFlags::lvalue_ref ? std::is_constructible_v<T, T &> : std::is_constructible_v<T, T &&>;
        }
    }
}
