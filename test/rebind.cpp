#include <em/meta/rebind.h>

#include <type_traits>

template <typename T>
struct A {};

struct B
{
    template <typename T>
    struct rebind {using other = A<T>;};
};

template <typename T, typename ...P>
struct C {};

template <typename T, typename ...P>
struct D
{
    template <typename U>
    struct rebind {using other = A<U>;};
};

// Only member rebind.
static_assert(std::is_same_v<em::Meta::rebind_to<int, B>, A<int>>);
// Only template parameter.
static_assert(std::is_same_v<em::Meta::rebind_to<int, C<double, float>>, C<int, float>>);
// Member rebind has precedence over template parameter.
static_assert(std::is_same_v<em::Meta::rebind_to<int, D<double, float>>, A<int>>);

// Check multi-parameter rebind.
template <typename T, typename ...P>
concept CanRebind = requires{typename em::Meta::rebind_to<T, P...>;};
static_assert(CanRebind<int, A<int>, D<double, float>>); // Same types ok.
static_assert(!CanRebind<int, A<int>, C<double, float>>); // Different types not ok.
