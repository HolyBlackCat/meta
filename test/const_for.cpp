#include "em/meta/const_for.h"

static constexpr int test_array[3]{4, 5, 6};

#define YES(...) __VA_ARGS__
#define NO(...)

// Sanity check for all the loop backends:
#define CHECK_BACKEND(Backend, NumberOrder, NonConsteval, ...) \
    static_assert((em::Meta::ConstForEach<Backend>([]<typename>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend, int, float, double>([]<typename>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend, 10, 20, 30>([]<auto>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend>(em::Meta::TypeList<>{}, []<typename>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend>(em::Meta::ValueList<>{}, []<auto>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend>(em::Meta::TypeList<int, float, double>{}, []<typename>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstForEach<Backend>(em::Meta::ValueList<10, 20, 30>{}, []<auto>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstFor<Backend, 0>([]<auto>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::ConstFor<Backend, 3>([]<auto>{__VA_ARGS__}), true)); \
    static_assert((em::Meta::RunEachFunc<Backend>(), true)); \
    static_assert((em::Meta::RunEachFunc<Backend>([]{__VA_ARGS__}), true)); \
    static_assert((em::Meta::RunEachFunc<Backend>([]{__VA_ARGS__}, []{__VA_ARGS__}), true)); \
    NonConsteval( \
        static_assert((em::Meta::ForEach<Backend>(test_array, test_array, [](int){__VA_ARGS__}), true)); \
        static_assert((em::Meta::ForEach<Backend>(test_array, test_array + 3, [](int){__VA_ARGS__}), true)); \
        static_assert([]{int ret = 0; em::Meta::ConstForEach<Backend, 4,5,6>([&]<auto I>{ret = ret * 10 + I; __VA_ARGS__}); return ret;}() == NumberOrder); \
        static_assert([]{int ret = 0; em::Meta::RunEachFunc<Backend>([&]{ret = ret * 10 + 4; __VA_ARGS__}, [&]{ret = ret * 10 + 5; __VA_ARGS__}, [&]{ret = ret * 10 + 6; __VA_ARGS__}); return ret;}() == NumberOrder); \
        static_assert([]{int ret = 0; em::Meta::ForEach<Backend>(test_array, test_array + 3, [&](int i){ret = ret * 10 + i; __VA_ARGS__}); return ret;}() == NumberOrder); \
    )

CHECK_BACKEND(em::Meta::LoopSimple                     , 456, YES)
CHECK_BACKEND(em::Meta::LoopSimple_Reverse             , 654, YES)
CHECK_BACKEND(em::Meta::LoopAnyOf<int>                 , 456, YES, return false;)
CHECK_BACKEND(em::Meta::LoopAnyOf_Reverse<int>         , 654, YES, return false;)
CHECK_BACKEND(em::Meta::LoopAnyOfConsteval<int>        , 456, NO , return false;)
CHECK_BACKEND(em::Meta::LoopAnyOfConsteval_Reverse<int>, 654, NO , return false;)
CHECK_BACKEND(em::Meta::LoopAnyOf<int>                 , 4  , YES, return true;)
CHECK_BACKEND(em::Meta::LoopAnyOf_Reverse<int>         , 6  , YES, return true;)
CHECK_BACKEND(em::Meta::LoopAnyOfConsteval<int>        , 4  , NO , return true;)
CHECK_BACKEND(em::Meta::LoopAnyOfConsteval_Reverse<int>, 6  , NO , return true;)



// Reversal tests.
static_assert(std::is_same_v<em::Meta::LoopSimple::reverse, em::Meta::LoopSimple_Reverse>);
static_assert(std::is_same_v<em::Meta::LoopSimple_Reverse::reverse, em::Meta::LoopSimple>);
static_assert(std::is_same_v<em::Meta::LoopAnyOf<int>::reverse, em::Meta::LoopAnyOf_Reverse<int>>);
static_assert(std::is_same_v<em::Meta::LoopAnyOf_Reverse<int>::reverse, em::Meta::LoopAnyOf<int>>);
static_assert(std::is_same_v<em::Meta::LoopAnyOfConsteval<int>::reverse, em::Meta::LoopAnyOfConsteval_Reverse<int>>);
static_assert(std::is_same_v<em::Meta::LoopAnyOfConsteval_Reverse<int>::reverse, em::Meta::LoopAnyOfConsteval<int>>);


// Misc tests:

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
