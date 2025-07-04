#include "em/meta/casts.h"

#include <type_traits>
#include <utility>


// --- static_cast_or_return_ref()

static_assert(std::is_same_v<decltype(em::Meta::static_cast_or_return_ref<int>(std::declval<float &>())), int>);

struct Base {};
struct Derived : Base
{
    Derived() {} // Disable construction from base (this would be an aggregate otherwise).
};

static_assert(std::is_same_v<decltype(em::Meta::static_cast_or_return_ref<Base>(std::declval<Derived &>())), Base &>);
static_assert(std::is_same_v<decltype(em::Meta::static_cast_or_return_ref<Base>(std::declval<const Derived &>())), const Base &>);
static_assert(std::is_same_v<decltype(em::Meta::static_cast_or_return_ref<Base>(std::declval<Derived &&>())), Base &&>);
static_assert(std::is_same_v<decltype(em::Meta::static_cast_or_return_ref<Base>(std::declval<const Derived &&>())), const Base &&>);

template <typename From, typename To>
concept C = requires{em::Meta::static_cast_or_return_ref<To>(std::declval<From &&>());};

static_assert(C<Derived &, Base>);
static_assert(!C<Base &, Derived>);
