#include "em/meta/common.h"
#include "em/meta/cvref_extras.h"

// copy_cv, copy_cvref

#define TEST(CV1, CV2, CV12) \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int   , CV2 float   >, CV12 float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int   , CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int   , CV2 float &&>, CV2  float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int & , CV2 float   >, CV12 float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int & , CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int & , CV2 float &&>, CV2  float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int &&, CV2 float   >, CV12 float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int &&, CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv           <CV1 int &&, CV2 float &&>, CV2  float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int   , CV2 float   >, CV12 float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int   , CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int   , CV2 float &&>, CV2  float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int & , CV2 float   >, CV12 float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int & , CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int & , CV2 float &&>, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int &&, CV2 float   >, CV12 float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int &&, CV2 float & >, CV2  float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref        <CV1 int &&, CV2 float &&>, CV2  float &&>); \

TEST(              ,               ,               )
TEST(const         ,               , const         )
TEST(      volatile,               ,       volatile)
TEST(const volatile,               , const volatile)
TEST(              , const         , const         )
TEST(const         , const         , const         )
TEST(      volatile, const         , const volatile)
TEST(const volatile, const         , const volatile)
TEST(              ,       volatile,       volatile)
TEST(const         ,       volatile, const volatile)
TEST(      volatile,       volatile,       volatile)
TEST(const volatile,       volatile, const volatile)
TEST(              , const volatile, const volatile)
TEST(const         , const volatile, const volatile)
TEST(      volatile, const volatile, const volatile)
TEST(const volatile, const volatile, const volatile)
#undef TEST

static_assert(std::is_same_v<em::Meta::const_type<               int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::const_type<const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::const_type<      volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::const_type<const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::const_type<               int & >,                int & >);
static_assert(std::is_same_v<em::Meta::const_type<const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::const_type<      volatile int & >,       volatile int & >);
static_assert(std::is_same_v<em::Meta::const_type<const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::const_type<               int &&>,                int &&>);
static_assert(std::is_same_v<em::Meta::const_type<const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::const_type<      volatile int &&>,       volatile int &&>);
static_assert(std::is_same_v<em::Meta::const_type<const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::maybe_const<false,                int   >,                int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<false,       volatile int   >,       volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<false,                int & >,                int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<false,       volatile int & >,       volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<false,                int &&>,                int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<false,       volatile int &&>,       volatile int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<false, const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::maybe_const<true,                int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<true,       volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const<true,                int & >,                int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<true,       volatile int & >,       volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const<true,                int &&>,                int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<true,       volatile int &&>,       volatile int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const<true, const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::const_type_cvref<               int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<      volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<               int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<      volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::const_type_cvref<               int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::const_type_cvref<      volatile int &&>, const volatile int &&>);
static_assert(std::is_same_v<em::Meta::const_type_cvref<const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,                int   >,                int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,       volatile int   >,       volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,                int & >,                int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,       volatile int & >,       volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,                int &&>,                int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false,       volatile int &&>,       volatile int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<false, const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,                int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const          int   >, const          int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,       volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const volatile int   >, const volatile int   >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,                int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,       volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,                int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true,       volatile int &&>, const volatile int &&>);
static_assert(std::is_same_v<em::Meta::maybe_const_cvref<true, const volatile int &&>, const volatile int &&>);



// ---



static_assert(em::Meta::SameTypeAndConstructibleFromDesc<int>(em::Meta::type_to_desc<int>));
static_assert(em::Meta::SameTypeAndConstructibleFromDesc<int &>(em::Meta::type_to_desc<int &>));
static_assert(!em::Meta::SameTypeAndConstructibleFromDesc<int &>(em::Meta::type_to_desc<int &&>));
static_assert(!em::Meta::SameTypeAndConstructibleFromDesc<int>(em::Meta::type_to_desc<float>));
