#include "em/meta/lists.h"

// --- list_subtract_ordered:

// types:
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, double>>, em::Meta::TypeList<float>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<float, int>>, em::Meta::TypeList<int, double>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<float, int>, em::Meta::TypeList<double>>, em::Meta::TypeList<int>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, float>, em::Meta::TypeList<double>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, double>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<int, float, double>>, em::Meta::TypeList<int, float, double>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<>, em::Meta::TypeList<int, float>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
// values:
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<1, 3>>, em::Meta::ValueList<2>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<2, 1>>, em::Meta::ValueList<1, 3>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<2, 1>, em::Meta::ValueList<3>>, em::Meta::ValueList<1>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<1, 2>, em::Meta::ValueList<3>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<>>, em::Meta::ValueList<1, 2, 3>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<1, 2, 3>>, em::Meta::ValueList<1, 2, 3>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<>, em::Meta::ValueList<1, 2>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::list_subtract_ordered<em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);


// --- list_reverse:
static_assert(std::is_same_v<em::Meta::list_reverse<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::list_reverse<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::list_reverse<em::Meta::TypeList<int, float, double>>, em::Meta::TypeList<double, float, int>>);
static_assert(std::is_same_v<em::Meta::list_reverse<em::Meta::ValueList<10, 20, 30>>, em::Meta::ValueList<30, 20, 10>>);
