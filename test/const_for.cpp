#include "em/meta/const_for.h"

static_assert(std::is_void_v<decltype(em::Meta::ConstForEx<4>([]<int i>{}))>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<0>([]<int i>{})), em::Meta::NoElements<false>>);

static_assert(em::Meta::ConstForEx<4>([]<int i>{if constexpr (i == 2) return i; else return 0;}) == 2);
static_assert(em::Meta::ConstForEx<4>([]<int i>{if constexpr (i == 2) return std::true_type{};}));

static constexpr int x = 42;
static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<4>([]<int i> -> const int & {return x;})), const int &>);

template <int I>
struct A
{
    constexpr operator bool() const {return I == 2;}
    friend bool operator==(A, A) = default;
};
static_assert(em::Meta::ConstForEx<4>([]<int i>{return A<i>{};}) == A<2>{});

static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<4>([]<int i>{
    if constexpr (i == 1)
        return A<1>{};
    else if constexpr (i == 2)
        return em::Meta::NoElements<false>{};
    else if constexpr (i == 3)
        return em::Meta::NoElements<true>{};
})), A<1>>);

static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<4>([]<int i>{
    if constexpr (i == 1)
        return A<1>{};
    else if constexpr (i == 2)
        return em::Meta::NoElements<false>{};
    else if constexpr (i == 3)
        return em::Meta::NoElements<false>{};
})), A<1>>);

static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<4>([]<int i>{
    if constexpr (i == 1)
        return A<1>{};
    else if constexpr (i == 3)
        return em::Meta::NoElements<false>{};
})), void>);

static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<4>([]<int i>{
    if constexpr (i == 1)
        return A<1>{};
    else if constexpr (i == 2)
        return em::Meta::NoElements<false>{};
    if constexpr (i == 3)
        return A<3>{};
})), A<3>>);

static_assert(std::is_same_v<decltype(em::Meta::ConstForEx<2>([]<int i>{
    return em::Meta::NoElements<false>{};
})), em::Meta::NoElements<false>>);


// ---


static_assert(std::is_void_v<decltype(em::Meta::ConstForEachEx<int, float, double>([]<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachEx<1,2,3>([]<int i>{}))>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEachEx<>([]<typename>{})), em::Meta::NoElements<false>>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEachEx<>([]<int i>{})), em::Meta::NoElements<false>>);

static_assert(std::is_void_v<decltype(em::Meta::ConstForEachEx(em::Meta::TypeList<int, float, double>{}, []<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachEx(em::Meta::ValueList<1,2,3>{}, []<int i>{}))>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEachEx(em::Meta::TypeList{}, []<typename>{})), em::Meta::NoElements<false>>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEachEx(em::Meta::ValueList{}, []<int i>{})), em::Meta::NoElements<false>>);
