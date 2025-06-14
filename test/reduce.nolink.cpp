#include "em/meta/reduce.h"

#include <type_traits>

template <typename A, typename B>
using X = std::integral_constant<int, A::value + B::value>;

static_assert(std::is_same_v<em::Meta::reduce_types<X, int>, int>);
static_assert(std::is_same_v<em::Meta::reduce_types<X, std::integral_constant<int,10>, std::integral_constant<int,20>>, std::integral_constant<int,30>>);
static_assert(std::is_same_v<em::Meta::reduce_types<X, std::integral_constant<int,10>, std::integral_constant<int,20>, std::integral_constant<int,30>>, std::integral_constant<int,60>>);

template <typename A, typename B>
struct Y
{
    using type = std::integral_constant<int, A::value + B::value>;
};

static_assert(std::is_same_v<em::Meta::reduce_types_indirect<Y, int>, int>);
static_assert(std::is_same_v<em::Meta::reduce_types_indirect<Y, std::integral_constant<int,10>, std::integral_constant<int,20>>, std::integral_constant<int,30>>);
static_assert(std::is_same_v<em::Meta::reduce_types_indirect<Y, std::integral_constant<int,10>, std::integral_constant<int,20>, std::integral_constant<int,30>>, std::integral_constant<int,60>>);
