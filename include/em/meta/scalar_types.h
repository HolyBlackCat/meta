#pragma once

#include "em/meta/common.h"

#include <cstddef>
#include <concepts>
#include <type_traits>

// This provides some helpers for the standard scalar types.
// Some customization points could be added here later if needed.

namespace em::Meta::Scalars
{
    template <typename T>
    concept CvrefUnqualifiedFloatingPoint = cvref_unqualified<T> && std::floating_point<T>;

    template <typename T>
    concept CvrefUnqualifiedIntegral = cvref_unqualified<T> && std::integral<T>;

    // An integral type other than `bool`.
    template <typename T>
    concept CvrefUnqualifiedIntegralNonBool = CvrefUnqualifiedIntegral<T> && !std::same_as<T, bool>;

    template <typename T>
    concept CvrefUnqualifiedScalar = CvrefUnqualifiedIntegral<T> || CvrefUnqualifiedFloatingPoint<T>;


    namespace detail::ScalarTypes
    {
        // Returns a sequential rank of a signed integral type.
        template <typename T> struct SignedIntegralRank {};
        // Maps back from a rank to a signed integral type.
        template <int I> struct SignedIntegralFromRank {};
        template <> struct SignedIntegralRank<signed char> : std::integral_constant<int, 0> {};
        template <> struct SignedIntegralFromRank<0> {using type = signed char;};
        template <> struct SignedIntegralRank<short> : std::integral_constant<int, 1> {};
        template <> struct SignedIntegralFromRank<1> {using type = short;};
        template <> struct SignedIntegralRank<int> : std::integral_constant<int, 2> {};
        template <> struct SignedIntegralFromRank<2> {using type = int;};
        template <> struct SignedIntegralRank<long> : std::integral_constant<int, 3> {};
        template <> struct SignedIntegralFromRank<3> {using type = long;};
        template <> struct SignedIntegralRank<long long> : std::integral_constant<int, 4> {};
        template <> struct SignedIntegralFromRank<4> {using type = long long;};
        // Disabling int128_t support for now, this makes it easier to test.
        // #ifdef __SIZEOF_INT128__
        // // `__int128_t` only exists on GCC and Clang. MSVC ships some classes to imitate large integers, but I'm not supporting them here for now.
        // template <> struct SignedIntegralRank<__int128_t> : std::integral_constant<int, 5> {};
        // template <> struct SignedIntegralFromRank<5> {using type = __int128_t;};
        // #endif

        template <bool Unsigned, typename T>
        struct MaybeMakeUnsigned {using type = T;};

        template <typename T>
        struct MaybeMakeUnsigned<true, T> {using type = std::make_unsigned_t<T>;};
    }

    // Returns a sequential rank of a given integer type. It's equal for signed and unsigned counterparts of the same type.
    // For `char` and `char??_t`, it matches the lowest rank of a type with the same size.
    // Note that `std::make_signed_t` is not SFINAE-friendly, so we have to constrain this manually using `IntegralNonBool`.
    template <CvrefUnqualifiedIntegralNonBool T>
    constexpr int integral_type_rank = detail::ScalarTypes::SignedIntegralRank<std::make_signed_t<T>>::value;

    // Is this a valid integral type rank? Those start from zero.
    template <int I>
    concept ValidIntegralTypeRank = requires{typename detail::ScalarTypes::SignedIntegralFromRank<I>::type;};

    // Returns an [un]signed integral type of the given sequential rank.
    // Usually roundtrips `integral_type_rank<T>`, except that this forces the type to be signed or unsigned, and always changes `char` and `char??_t` to other types.
    template <int I, bool Unsigned> requires ValidIntegralTypeRank<I>
    using MaybeUnsignedIntegralTypeFromRank = typename detail::ScalarTypes::MaybeMakeUnsigned<Unsigned, typename detail::ScalarTypes::SignedIntegralFromRank<I>::type>::type;

    template <int I> requires ValidIntegralTypeRank<I>
    using SignedIntegralTypeFromRank = MaybeUnsignedIntegralTypeFromRank<I, false>;

    template <int I> requires ValidIntegralTypeRank<I>
    using UnsignedIntegralTypeFromRank = MaybeUnsignedIntegralTypeFromRank<I, true>;


    // Replaces `char` and `char??_t` with their underlying types.
    template <CvrefUnqualifiedIntegralNonBool T>
    using NormalizeIntegralType = MaybeUnsignedIntegralTypeFromRank<integral_type_rank<T>, std::unsigned_integral<T>>;


    // Increases an integral type rank by one, preserving its signedness.
    // This doesn't necessarily increase the `sizeof`.
    template <CvrefUnqualifiedIntegralNonBool T> requires ValidIntegralTypeRank<integral_type_rank<T> + 1>
    using IncrementIntegralRank = MaybeUnsignedIntegralTypeFromRank<integral_type_rank<T> + 1, std::is_unsigned_v<T>>;


    namespace detail::ScalarTypes
    {
        template <typename T>
        struct PromoteWithSameSign {using type = T;};
        template <>
        struct PromoteWithSameSign<bool> {using type = bool;};
        template <std::signed_integral T> requires(sizeof(T) < sizeof(int))
        struct PromoteWithSameSign<T> {using type = int;};
        template <std::unsigned_integral T> requires (sizeof(T) < sizeof(int))
        struct PromoteWithSameSign<T> {using type = unsigned int;};
    }

    // If `T` is an integral non-bool type smaller than `int`, promotes it to either `int` or `unsigned int`, keeping the original signedness.
    template <cvref_unqualified T>
    using PromoteWithSameSign = typename detail::ScalarTypes::PromoteWithSameSign<T>::type;

    namespace detail::ScalarTypes
    {
        template <typename T>
        concept CanIncrementIntegralRank = ValidIntegralTypeRank<integral_type_rank<T> + 1>;

        template <CanIncrementIntegralRank T>
        struct IntegralWithLargerSize {using type = IncrementIntegralRank<T>;};

        // Increment recursively until the size increases.
        template <CanIncrementIntegralRank T>
        requires (sizeof(T) == sizeof(IncrementIntegralRank<T>)) && requires{typename IntegralWithLargerSize<IncrementIntegralRank<T>>::type;}
        struct IntegralWithLargerSize<T> : IntegralWithLargerSize<IncrementIntegralRank<T>> {};
    }

    // Bumps the type to the closest larger type of the same signedness.
    // Unlike `IncrementIntegralRank`, this is guaranteed to increase `sizeof`.
    template <CvrefUnqualifiedIntegralNonBool T> requires requires{typename detail::ScalarTypes::IntegralWithLargerSize<T>::type;}
    using IncreaseIntegralSize = typename detail::ScalarTypes::IntegralWithLargerSize<T>::type;

    namespace detail::ScalarTypes
    {
        // Returns the largest floating-point type in `P...`, ignoring non-FP types. Returns void if no FP types, but we don't rely on this.
        // If the result conflicts between several different FP types of the same size (e.g. `float` vs `std::float32_t`, once compilers support them), gracefully fails.
        // Note that we need to do this check using `ConflictingSizeof` instead of more naively (in the folding step), to make the result order-independent.
        template <typename R /*= void*/, std::size_t ConflictingSizeof /*= 0*/, typename ...P>
        struct LargerTypeIgnoringNonFp {};
        // The final step of the recursion, return `R` if it's not void.
        template <typename R, std::size_t ConflictingSizeof> requires (!std::is_void_v<R> && sizeof(R) != ConflictingSizeof)
        struct LargerTypeIgnoringNonFp<R, ConflictingSizeof> {using type = R;};
        // Any type + non-fp = keep existing result.
        template <typename R, std::size_t ConflictingSizeof, typename T, typename ...P> requires(!CvrefUnqualifiedFloatingPoint<T>)
        struct LargerTypeIgnoringNonFp<R, ConflictingSizeof, T, P...> : LargerTypeIgnoringNonFp<R, ConflictingSizeof, P...> {};
        // Void (indicating no previous FP) + FP = use this new FP.
        template <std::size_t ConflictingSizeof, CvrefUnqualifiedFloatingPoint T, typename ...P>
        struct LargerTypeIgnoringNonFp<void, ConflictingSizeof, T, P...> : LargerTypeIgnoringNonFp<T, ConflictingSizeof, P...> {};
        // FP + FP = use the larger one.
        // This may eventually need FP type ranks (like integral ones), if we find a platform where `sizeof(float) == sizeof(double)`.
        template <CvrefUnqualifiedFloatingPoint R, std::size_t ConflictingSizeof, CvrefUnqualifiedFloatingPoint T, typename ...P>
        struct LargerTypeIgnoringNonFp<R, ConflictingSizeof, T, P...> : LargerTypeIgnoringNonFp<std::conditional_t<(sizeof(R) > sizeof(T)), R, T>, (sizeof(R) == sizeof(T) ? sizeof(R) : ConflictingSizeof), P...> {};

        // Returns the largest integral type in `P...`. Bools are considered to have the lowest rank.
        // May apply `NormalizeIntegralType` to the result, if we had multiple equally large types that could be the result.
        template <typename R /*= bool*/, std::size_t NormalizeSizeof /*= 0*/, typename ...P>
        struct LargerTypeIntegral {};
        // Return the final result.
        template <typename R, std::size_t NormalizeSizeof> requires(sizeof(R) != NormalizeSizeof)
        struct LargerTypeIntegral<R, NormalizeSizeof> {using type = R;};
        template <typename R, std::size_t NormalizeSizeof> requires(sizeof(R) == NormalizeSizeof)
        struct LargerTypeIntegral<R, NormalizeSizeof> {using type = NormalizeIntegralType<R>;};
        // bool + bool = bool
        template <std::size_t NormalizeSizeof, typename ...P>
        struct LargerTypeIntegral<bool, NormalizeSizeof, bool, P...> : LargerTypeIntegral<bool, NormalizeSizeof, P...> {};
        // bool + T = T
        template <std::size_t NormalizeSizeof, typename T, typename ...P>
        struct LargerTypeIntegral<bool, NormalizeSizeof, T, P...> : LargerTypeIntegral<T, NormalizeSizeof, P...> {};
        // T + bool = T
        template <typename R, std::size_t NormalizeSizeof, typename ...P>
        struct LargerTypeIntegral<R, NormalizeSizeof, bool, P...> : LargerTypeIntegral<R, NormalizeSizeof, P...> {};
        // T + T = ??
        template <typename R, std::size_t NormalizeSizeof, typename T, typename ...P>
        struct LargerTypeIntegral<R, NormalizeSizeof, T, P...> : LargerTypeIntegral<NormalizeIntegralType<std::conditional_t<(integral_type_rank<R> > integral_type_rank<T>), R, T>>, (integral_type_rank<R> == integral_type_rank<T> ? sizeof(R) : NormalizeSizeof), P...> {};

        // Returns true if the max size of all unsigned types in the list (ignoring `bool`) is strictly less than the max size of all signed types in the list.
        // There must be at least one signed and one unsigned type in the list.
        template <std::size_t UnsignedSize /*=0*/, std::size_t SignedSize /*=0*/, typename ...P>
        struct UnsignedSizeSmallerThanSigned : std::bool_constant<(UnsignedSize < SignedSize)> {};
        // Skip `bool`.
        template <std::size_t UnsignedSize, std::size_t SignedSize, typename ...P>
        struct UnsignedSizeSmallerThanSigned<UnsignedSize, SignedSize, bool, P...> : UnsignedSizeSmallerThanSigned<UnsignedSize, SignedSize, P...> {};
        // Unsigned but not `bool`.
        template <std::size_t UnsignedSize, std::size_t SignedSize, std::unsigned_integral T, typename ...P>
        struct UnsignedSizeSmallerThanSigned<UnsignedSize, SignedSize, T, P...> : UnsignedSizeSmallerThanSigned<(sizeof(T) > UnsignedSize ? sizeof(T) : UnsignedSize), SignedSize, P...> {};
        // Signed.
        template <std::size_t UnsignedSize, std::size_t SignedSize, std::signed_integral T, typename ...P>
        struct UnsignedSizeSmallerThanSigned<UnsignedSize, SignedSize, T, P...> : UnsignedSizeSmallerThanSigned<UnsignedSize, (sizeof(T) > SignedSize ? sizeof(T) : SignedSize), P...> {};

        // Given an unsigned type other than `bool`, makes it signed and increases the size. Gracefully fails if it's impossible to make the type larger.
        // Given a signed type, returns it unchanged.
        template <typename T>
        struct BumpIntegralToSigned {};
        template <>
        struct BumpIntegralToSigned<bool> {using type = bool;};
        template <std::signed_integral T>
        struct BumpIntegralToSigned<T> {using type = T;};
        template <std::unsigned_integral T> requires (!std::is_same_v<T, bool>) && CanIncrementIntegralRank<T>
        struct BumpIntegralToSigned<T> {using type = IncreaseIntegralSize<std::make_signed_t<T>>;};


        // Read those in reverse.

        // If all types are integral (of different signedness, if we reached this point), bump all unsigned types to signed and try again.
        template <bool AllowBiggerTypes, typename ...P>
        struct LargerTypeDifferentSignedness {};
        template <bool AllowBiggerTypes, typename ...P>
        requires
            (CvrefUnqualifiedIntegral<P> && ...) &&
            (AllowBiggerTypes || UnsignedSizeSmallerThanSigned<0, 0, P...>::value) &&
            (requires{typename BumpIntegralToSigned<P>::type;} && ...)
        struct LargerTypeDifferentSignedness<AllowBiggerTypes, P...> : LargerTypeIntegral<bool, 0, typename BumpIntegralToSigned<P>::type...> {};

        // If all types are integral with the same signedness (ignoring `bool`s), return the largest type by rank.
        template <bool AllowBiggerTypes, typename ...P>
        struct LargerTypeSameSignedness : LargerTypeDifferentSignedness<AllowBiggerTypes, P...> {};
        template <bool AllowBiggerTypes, typename ...P> requires (CvrefUnqualifiedIntegral<P> && ...) && (((std::signed_integral<P> || std::is_same_v<P, bool>) && ...) || ((std::unsigned_integral<P> || std::is_same_v<P, bool>) && ...))
        struct LargerTypeSameSignedness<AllowBiggerTypes, P...> : LargerTypeIntegral<bool, 0, P...> {};

        // If all types are scalar and at least one is floating-point, returns the largest floating-point type.
        template <bool AllowBiggerTypes, typename ...P>
        struct LargerTypeFp : LargerTypeSameSignedness<AllowBiggerTypes, P...> {};
        template <bool AllowBiggerTypes, typename ...P> requires (CvrefUnqualifiedScalar<P> && ...) && (CvrefUnqualifiedFloatingPoint<P> || ...)
        struct LargerTypeFp<AllowBiggerTypes, P...> : LargerTypeIgnoringNonFp<void, 0, P...> {};

        // If all types are the same, returns that type.
        template <bool AllowBiggerTypes, typename ...P>
        struct LargerTypeBase : LargerTypeFp<AllowBiggerTypes, P...> {};
        template <bool AllowBiggerTypes, typename ...P> requires same_as_all<P...>
        struct LargerTypeBase<AllowBiggerTypes, P...> {using type = P...[0];};
    }

    // A convenience concept, checks that `LargerType<...>` below is valid.
    template <bool AllowBiggerTypes, typename ...P>
    concept HaveLargerType = sizeof...(P) > 0 && requires{typename detail::ScalarTypes::LargerTypeBase<AllowBiggerTypes, P...>::type;};

    // Returns the largest type among `P...`.
    // If any of the types are non-scalar, then they must all be the same, and that type is returned.
    // If any of the types are floating-point, then the largest floating-point type wins (or a SFINAE error on an ambiguity, such as between `float` and `std::float32_t`).
    // Then if all types are integral and have the same signedness (ignoring `bool`s), return the largest type by rank (i.e. basically by `sizeof`,
    //   except that e.g. `int` is always considered to be less than `long`, regardless of size), again ignoring bools.
    // If the integral types have different signedness (ignoring `bool`s), then the unsigned ones are bumped to next larger signed types, and again the largest type by rank is chosen.
    // If `AllowBiggerTypes == false`, then refuses to return a type larger than all input types and soft errors instead. (Such larger type can only be caused by bumping an unsigned type,
    //   e.g. `int` + `unsigned int` = `std::int64_t` only works with `AllowBiggerTypes == true`).
    template <bool AllowBiggerTypes, cvref_unqualified ...P> requires HaveLargerType<AllowBiggerTypes, P...>
    using LargerType = typename detail::ScalarTypes::LargerTypeBase<AllowBiggerTypes, P...>::type;
}
