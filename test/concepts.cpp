#include "em/meta/concepts.h"

static_assert(em::Meta::truthy_type<std::true_type>);
static_assert(em::Meta::falsey_type<std::false_type>);
static_assert(em::Meta::falsey_type<std::nullptr_t>);

static_assert(!em::Meta::truthy_type<std::false_type &>);
static_assert(!em::Meta::truthy_type<std::true_type &>);
static_assert(!em::Meta::falsey_type<std::nullptr_t &>);
static_assert(!em::Meta::truthy_type<std::false_type &&>);
static_assert(!em::Meta::truthy_type<std::true_type &&>);
static_assert(!em::Meta::falsey_type<std::nullptr_t &&>);
