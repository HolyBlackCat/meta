#include "em/macros/meta/detectable_base.h"
#include "em/macros/meta/enclosing_class.h"
#include "em/macros/portable/warnings.h"
#include "em/meta/detect_bases.h"

struct Tag {};
#define BASE EM_TYPEDEF_ENCLOSING_CLASS(Self) EM_DETECTABLE_BASE((Tag), (Self))

struct AA
{
    BASE
    struct A {BASE};
};

struct B : virtual AA::A
{
    BASE
};

struct C0 {BASE};

struct C : virtual AA::A, C0
{
    BASE
};

struct D : B, C
{
    BASE
};

static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::AllBasesFlat                <Tag, D>, em::Meta::TypeList<AA::A, B, C0, C    >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::AllBasesFlatAndSelf         <Tag, D>, em::Meta::TypeList<AA::A, B, C0, C, D >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::VirtualBasesFlat            <Tag, D>, em::Meta::TypeList<AA::A              >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::VirtualBasesFlatAndSelf     <Tag, D>, em::Meta::TypeList<AA::A, D           >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::NonVirtualBasesFlat         <Tag, D>, em::Meta::TypeList<B, C0, C           >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::NonVirtualBasesFlatAndSelf  <Tag, D>, em::Meta::TypeList<B, C0, C, D        >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::NonVirtualBasesDirect       <Tag, D>, em::Meta::TypeList<B, C               >>);
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::NonVirtualBasesDirectAndSelf<Tag, D>, em::Meta::TypeList<B, C, D            >>);

// Edge cases:

// 1. Direct ambiguous base is skipped.
struct X1 : AA::A {BASE};
#if EM_IS_GCC_LIKE
EM_DIAGNOSTICS_PUSH
EM_DIAGNOSTICS_IGNORE("-Winaccessible-base")
#endif
struct X2 : X1, AA::A {}; // Yes, Clang warns here.
#if EM_IS_GCC_LIKE
EM_DIAGNOSTICS_POP
#endif
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::NonVirtualBasesDirect<Tag, X2>, em::Meta::TypeList<X1>>);

// Both virtual and non-virtual base with the same name causes the virtual one to get skipped.
struct X3 : B, X1 {BASE};
static_assert(em::Meta::lists_have_same_elems_and_size<em::Meta::DetectBases::VirtualBasesFlat<Tag, X3>, em::Meta::TypeList<>>);
