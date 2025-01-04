#include "em/meta/qualifiers.h"

// copy_cv, copy_cvref

#define TEST(CV, CV2) \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int   , CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int   , CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int   , CV2 float &&>, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int & , CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int & , CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int & , CV2 float &&>, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int &&, CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int &&, CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv   <CV int &&, CV2 float &&>, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int   , CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int   , CV2 float & >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int   , CV2 float &&>, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int & , CV2 float   >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int & , CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int & , CV2 float &&>, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int &&, CV2 float   >, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int &&, CV2 float & >, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cvref<CV int &&, CV2 float &&>, CV float &&>); \

TEST(              ,               )
TEST(const         ,               )
TEST(      volatile,               )
TEST(const volatile,               )
TEST(              , const         )
TEST(const         , const         )
TEST(      volatile, const         )
TEST(const volatile, const         )
TEST(              ,       volatile)
TEST(const         ,       volatile)
TEST(      volatile,       volatile)
TEST(const volatile,       volatile)
TEST(              , const volatile)
TEST(const         , const volatile)
TEST(      volatile, const volatile)
TEST(const volatile, const volatile)
#undef TEST

static_assert(std::is_same_v<em::Meta::cv_const<               int   >, const          int>);
static_assert(std::is_same_v<em::Meta::cv_const<const          int   >, const          int>);
static_assert(std::is_same_v<em::Meta::cv_const<      volatile int   >, const volatile int>);
static_assert(std::is_same_v<em::Meta::cv_const<const volatile int   >, const volatile int>);
static_assert(std::is_same_v<em::Meta::cv_const<               int & >,                int & >);
static_assert(std::is_same_v<em::Meta::cv_const<const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::cv_const<      volatile int & >,       volatile int & >);
static_assert(std::is_same_v<em::Meta::cv_const<const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::cv_const<               int &&>,                int &&>);
static_assert(std::is_same_v<em::Meta::cv_const<const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::cv_const<      volatile int &&>,       volatile int &&>);
static_assert(std::is_same_v<em::Meta::cv_const<const volatile int &&>, const volatile int &&>);

static_assert(std::is_same_v<em::Meta::cvref_const<               int   >, const          int>);
static_assert(std::is_same_v<em::Meta::cvref_const<const          int   >, const          int>);
static_assert(std::is_same_v<em::Meta::cvref_const<      volatile int   >, const volatile int>);
static_assert(std::is_same_v<em::Meta::cvref_const<const volatile int   >, const volatile int>);
static_assert(std::is_same_v<em::Meta::cvref_const<               int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::cvref_const<const          int & >, const          int & >);
static_assert(std::is_same_v<em::Meta::cvref_const<      volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::cvref_const<const volatile int & >, const volatile int & >);
static_assert(std::is_same_v<em::Meta::cvref_const<               int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::cvref_const<const          int &&>, const          int &&>);
static_assert(std::is_same_v<em::Meta::cvref_const<      volatile int &&>, const volatile int &&>);
static_assert(std::is_same_v<em::Meta::cvref_const<const volatile int &&>, const volatile int &&>);
