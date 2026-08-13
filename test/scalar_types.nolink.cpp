#include "em/meta/scalar_types.h"

#include <cstdint>
#include <type_traits>

static_assert(em::Meta::Scalars::integral_type_rank<char> == 0);
static_assert(em::Meta::Scalars::integral_type_rank<signed char> == 0);
static_assert(em::Meta::Scalars::integral_type_rank<unsigned char> == 0);
static_assert(em::Meta::Scalars::integral_type_rank<short> == 1);
static_assert(em::Meta::Scalars::integral_type_rank<unsigned short> == 1);
static_assert(em::Meta::Scalars::integral_type_rank<int> == 2);
static_assert(em::Meta::Scalars::integral_type_rank<unsigned int> == 2);
static_assert(em::Meta::Scalars::integral_type_rank<long> == 3);
static_assert(em::Meta::Scalars::integral_type_rank<unsigned long> == 3);
static_assert(em::Meta::Scalars::integral_type_rank<long long> == 4);
static_assert(em::Meta::Scalars::integral_type_rank<unsigned long long> == 4);
// #ifdef __SIZEOF_INT128__
// static_assert(em::Meta::Scalars::integral_type_rank<__int128_t> == 5);
// __extension__ static_assert(em::Meta::Scalars::integral_type_rank<unsigned __int128> == 5);
// #endif

static_assert(em::Meta::Scalars::integral_type_rank<char8_t> == 0);
static_assert(em::Meta::Scalars::integral_type_rank<char16_t> == em::Meta::Scalars::integral_type_rank<std::int16_t>);
static_assert(em::Meta::Scalars::integral_type_rank<char32_t> == em::Meta::Scalars::integral_type_rank<std::int32_t>); // This isn't required to be true in general, but usually should be true.

static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<0>, signed char>);
static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<0>, unsigned char>);
static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<1>, short>);
static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<1>, unsigned short>);
static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<2>, int>);
static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<2>, unsigned int>);
static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<3>, long>);
static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<3>, unsigned long>);
static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<4>, long long>);
static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<4>, unsigned long long>);
// #ifdef __SIZEOF_INT128__
// static_assert(std::is_same_v<em::Meta::Scalars::SignedIntegralTypeFromRank<5>, __int128_t>);
// __extension__ static_assert(std::is_same_v<em::Meta::Scalars::UnsignedIntegralTypeFromRank<5>, unsigned __int128>);
// #endif

static_assert(std::is_same_v<em::Meta::Scalars::MaybeUnsignedIntegralTypeFromRank<2, false>, int>);
static_assert(std::is_same_v<em::Meta::Scalars::MaybeUnsignedIntegralTypeFromRank<2, true>, unsigned int>);


static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<int>, long>);
static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<long>, long long>);
static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<unsigned int>, unsigned long>);
static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<unsigned long>, unsigned long long>);
static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<char>, std::conditional_t<std::is_signed_v<char>, short, unsigned short>>);
static_assert(std::is_same_v<em::Meta::Scalars::IncrementIntegralRank<char8_t>, unsigned short>);


static_assert(std::is_same_v<em::Meta::Scalars::IncreaseIntegralSize<char>, short>);
static_assert(std::is_same_v<em::Meta::Scalars::IncreaseIntegralSize<unsigned char>, unsigned short>);
static_assert(std::is_same_v<em::Meta::Scalars::IncreaseIntegralSize<int>, std::conditional_t<(sizeof(long) > sizeof(int)), long, long long>>);
static_assert(std::is_same_v<em::Meta::Scalars::IncreaseIntegralSize<unsigned int>, std::conditional_t<(sizeof(long) > sizeof(int)), unsigned long, unsigned long long>>);
// #ifdef __SIZEOF_INT128__
// static_assert(std::is_same_v<em::Meta::Scalars::IncreaseIntegralSize<long>, std::conditional_t<(sizeof(long) < sizeof(long long)), long long, __int128_t>>);
// #endif



// --- LargerType:

// Checks that both strict and relaxed `LargeType<...>` agree on the result.
template <typename ...P> requires std::is_same_v<em::Meta::Scalars::LargerType<false, P...>, em::Meta::Scalars::LargerType<true, P...>>
using LargerTypeCommon = em::Meta::Scalars::LargerType<true, P...>;

// Checks that the relaxed `LargeType<...>` works and the strict one doesn't.
template <typename ...P> requires em::Meta::Scalars::HaveLargerType<true, P...> && (!em::Meta::Scalars::HaveLargerType<false, P...>)
using LargerTypeOnlyWhenAllowBiggerTypes = em::Meta::Scalars::LargerType<true, P...>;

template <typename ...P>
constexpr bool NoLargerType = !em::Meta::Scalars::HaveLargerType<false, P...> && !em::Meta::Scalars::HaveLargerType<true, P...>;

static_assert(NoLargerType<>);

static_assert(std::is_same_v<LargerTypeCommon<char *, char *>, char *>); // We can support any type as long as it's exactly the same.
static_assert(NoLargerType<char *, void *>); // Nothing in common here (we don't have special treatment for pointers).

// Floating-point:
static_assert(std::is_same_v<LargerTypeCommon<float, float>, float>);
static_assert(std::is_same_v<LargerTypeCommon<float, double>, double>);
static_assert(std::is_same_v<LargerTypeCommon<double, float>, double>);

// Floating-point vs integral:
static_assert(std::is_same_v<LargerTypeCommon<float, int>, float>);
static_assert(std::is_same_v<LargerTypeCommon<int, float>, float>);
static_assert(std::is_same_v<LargerTypeCommon<float, int, double, long long>, double>);

// The result is not normalized when the types are the same.
static_assert(std::is_same_v<LargerTypeCommon<char, char>, char>);
// The types are normalized when they are different.
static_assert(std::is_same_v<LargerTypeCommon<char, signed char>, signed char>);
static_assert(std::is_same_v<LargerTypeCommon<signed char, char>, signed char>);
static_assert(std::is_same_v<LargerTypeCommon<unsigned char, char8_t>, unsigned char>);

static_assert(std::is_same_v<LargerTypeCommon<char, short>, short>);
static_assert(std::is_same_v<LargerTypeCommon<short, char>, short>);
static_assert(std::is_same_v<LargerTypeCommon<int, long>, long>); // Even if those have the same size, the "larger" type will be selected.
static_assert(std::is_same_v<LargerTypeCommon<long, int>, long>); // ^
static_assert(std::is_same_v<LargerTypeCommon<long, long long>, long long>); // ^
static_assert(std::is_same_v<LargerTypeCommon<long long, long>, long long>); // ^

// Mixed signedness:

// Unsigned is smaller.
static_assert(std::is_same_v<LargerTypeCommon<unsigned char, short>, short>);
static_assert(std::is_same_v<LargerTypeCommon<short, unsigned char>, short>);
static_assert(std::is_same_v<LargerTypeCommon<unsigned char, short>, short>);
static_assert(std::is_same_v<LargerTypeCommon<short, unsigned char>, short>);

// Notice how this works even on `Relaxed == false`. The first two elements can only be combined on relaxed mode, but having the third one makes this work.
static_assert(std::is_same_v<LargerTypeCommon<short, unsigned short, int>, int>);
static_assert(std::is_same_v<LargerTypeCommon<short, unsigned short, float>, float>);


// Same size.
static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<unsigned short, short>, int>);
static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<short, unsigned short>, int>);

static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<char, unsigned char>, short>);
static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<unsigned char, char>, short>);

// This is already the largest type, nowhere to grow to.
static_assert(NoLargerType<long long, unsigned long long>);
static_assert(NoLargerType<unsigned long long, long long>);


// Unsigned is larger.
static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<unsigned short, signed char>, int>);
static_assert(std::is_same_v<LargerTypeOnlyWhenAllowBiggerTypes<signed char, unsigned short>, int>);


// Bool:
static_assert(std::is_same_v<LargerTypeCommon<bool, bool>, bool>);
static_assert(std::is_same_v<LargerTypeCommon<bool, signed char>, signed char>);
static_assert(std::is_same_v<LargerTypeCommon<bool, unsigned char>, unsigned char>);
static_assert(std::is_same_v<LargerTypeCommon<bool, char>, char>); // This one doesn't really matter, but I guess keeping `char` is nice?
static_assert(std::is_same_v<LargerTypeCommon<signed char, bool, char>, signed char>); // Here we have normalization kick in.
// Now with the reverse argument order:
static_assert(std::is_same_v<LargerTypeCommon<signed char, bool>, signed char>);
static_assert(std::is_same_v<LargerTypeCommon<unsigned char, bool>, unsigned char>);
static_assert(std::is_same_v<LargerTypeCommon<char, bool>, char>); // This one doesn't really matter, but I guess keeping `char` is nice?
static_assert(std::is_same_v<LargerTypeCommon<char, bool, signed char>, signed char>); // Here we have normalization kick in.


// Can't test this at the moment, no support in Clang.
#ifdef __STDCPP_FLOAT32_T__
static_assert(std::is_same_v<LargerTypeCommon<float, std::float32_t, double>, double>); // This is not an ambiguity.
static_assert(NoLargerType<float, std::float32_t>, double>); // But this is.
#endif
