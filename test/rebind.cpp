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
static_assert(std::is_same_v<em::Meta::rebind<B, int>, A<int>>);
// Only template parameter.
static_assert(std::is_same_v<em::Meta::rebind<C<double, float>, int>, C<int, float>>);
// Member rebind has precedence over template parameter.
static_assert(std::is_same_v<em::Meta::rebind<D<double, float>, int>, A<int>>);
