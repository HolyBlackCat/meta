#include "em/meta/qualifiers.h"

// copy_cv, copy_cvref

#define TEST(CV, CV2) \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int   , CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int   , CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int   , CV2 float &&>, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int & , CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int & , CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int & , CV2 float &&>, CV float &&>); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int &&, CV2 float   >, CV float   >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int &&, CV2 float & >, CV float & >); \
    static_assert(std::is_same_v<em::Meta::copy_cv<CV int &&, CV2 float &&>, CV float &&>); \
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
