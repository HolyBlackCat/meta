#include "em/meta/const_for.h"

static_assert(std::is_void_v<decltype(em::Meta::ConstFor<em::Meta::LoopSimple, 0>([]<int i>{}))>);
static_assert(std::is_void_v<decltype(em::Meta::ConstFor<em::Meta::LoopSimple, 4>([]<int i>{}))>);

static_assert([]{
    int ret = 0;
    em::Meta::ConstFor<em::Meta::LoopSimple, 4>([&]<int i>{ret = ret * 10 + i + 1;});
    return ret;
}() == 1234);

static_assert([]{
    int ret = 0;
    em::Meta::ConstForEach<em::Meta::LoopSimple, 5, 6, 7, 8>([&]<int i>{ret = ret * 10 + i;});
    return ret;
}() == 5678);

static_assert(em::Meta::ConstForEach<em::Meta::LoopAnyOf<>, int, float, double>([]<typename T>{return std::is_same_v<T, float>;}));
static_assert(!em::Meta::ConstForEach<em::Meta::LoopAnyOf<>, int, float, double>([]<typename T>{return std::is_same_v<T, char>;}));

static_assert(std::is_same_v<decltype(em::Meta::ConstForEach<em::Meta::LoopAnyOfConsteval<>, int, float, double>([]<typename T>{return std::is_same<T, float>{};}))::type, std::true_type>);
static_assert(std::is_same_v<decltype(em::Meta::ConstForEach<em::Meta::LoopAnyOfConsteval<>>([]<typename T>{return std::is_same<T, float>{};})), std::nullptr_t>);

static_assert([]{
    int ret = 0;
    em::Meta::RunEachFunc<em::Meta::LoopSimple>([&]{ret += 10;}, [&]{ret += 20;});
    return ret;
}() == 30);

static_assert([]{
    int ret = 0;
    em::Meta::RunEachFunc<em::Meta::LoopSimple>([&]{ret += 1; return false;}, [&]{ret += 10; return true;}, [&]{ret += 100; return false;});
    return ret;
}() == 111);
static_assert([]{
    int ret = 0;
    em::Meta::RunEachFunc<em::Meta::LoopAnyOf<>>([&]{ret += 1; return false;}, [&]{ret += 10; return true;}, [&]{ret += 100; return false;});
    return ret;
}() == 11);

// #error test the return type of consteval RunEachFunc
