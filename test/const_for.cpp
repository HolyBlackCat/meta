#include "em/meta/const_for.h"

static_assert(std::is_void_v<decltype(em::Meta::ConstForAdvanced<4>([]<int i>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForAdvanced<0>([]<int i>{}))>);

static_assert(em::Meta::ConstForAdvanced<4>([]<int i>{if constexpr (i == 2) return i; else return 0;}) == 2);
static_assert(em::Meta::ConstForAdvanced<4>([]<int i>{if constexpr (i == 2) return std::true_type{};}));

static constexpr int x = 42;
static_assert(std::is_same_v<decltype(em::Meta::ConstForAdvanced<4>([]<int i> -> const int & {return x;})), const int &>);

template <int I>
struct A
{
    constexpr operator bool() const {return I == 2;}
    friend bool operator==(A, A) = default;
};
static_assert(em::Meta::ConstForAdvanced<4>([]<int i>{return A<i>{};}) == A<2>{});


// ---


static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced<int, float, double>([]<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced<1,2,3>([]<int i>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced<>([]<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced<>([]<int i>{}))>);

static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced(em::Meta::TypeList<int, float, double>{}, []<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced(em::Meta::ValueList<1,2,3>{}, []<int i>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced(em::Meta::TypeList{}, []<typename>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstForEachAdvanced(em::Meta::ValueList{}, []<int i>{}))>);
