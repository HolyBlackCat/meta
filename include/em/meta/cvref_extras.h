#pragma once

#include "em/macros/utils/implies.h"
#include "em/macros/utils/flag_enum.h"

#include <type_traits>
#include <typeinfo>

// Some extra stuff for manipulating cvref-qualifiers.

namespace em::Meta
{
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

    // Checks that `T` is constructible from `desc`, and both are the same type ignoring cvref-qualifiers.
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

    // Checks that `desc` is constructible from `T`, and both are the same type ignoring cvref-qualifiers.
    // If `T` is a non-ref, it's assumed to be an rvalue ref.
    template <typename T>
    [[nodiscard]] constexpr bool SameTypeAndDescIsConstructibleFrom(const CvrefFlagsAndType &desc)
    {
        if (*desc.type != typeid(T))
            return false;

        if (bool(desc.flags & CvrefFlags::ref_mask))
            return CvrefFlagsConvertible(cvref_to_flags<T>, desc.flags);
        else
            return std::is_constructible_v<std::remove_cvref_t<T>, T &&>;
    }
}
