#include "em/meta/lists.h"

// --- from:

template <typename ...P> struct TypePack {};
template <auto ...P> struct ValuePack {};
static_assert(std::is_same_v<em::Meta::List::from<TypePack<int, float>>, em::Meta::TypeList<int, float>>);
static_assert(std::is_same_v<em::Meta::List::from<ValuePack<10, 20>>, em::Meta::ValueList<10, 20>>);


// --- apply_types, apply_values:

static_assert(std::is_same_v<em::Meta::List::apply_types<TypePack, em::Meta::TypeList<int, float>>, TypePack<int, float>>);
static_assert(std::is_same_v<em::Meta::List::apply_values<ValuePack, em::Meta::ValueList<10, 20>>, ValuePack<10, 20>>);


// --- size:

static_assert(em::Meta::List::size<em::Meta::TypeList<>> == 0);
static_assert(em::Meta::List::size<em::Meta::TypeList<int, float>> == 2);
static_assert(em::Meta::List::size<em::Meta::ValueList<>> == 0);
static_assert(em::Meta::List::size<em::Meta::ValueList<10, 20>> == 2);


// --- reverse:

static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::TypeList<int, float, char>>, em::Meta::TypeList<char, float, int>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::ValueList<10, 20, 30>>, em::Meta::ValueList<30, 20, 10>>);


// --- append_types, append_values:

static_assert(std::is_same_v<em::Meta::List::append_types<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::append_types<em::Meta::TypeList<>, int, float>, em::Meta::TypeList<int, float>>);
static_assert(std::is_same_v<em::Meta::List::append_types<em::Meta::TypeList<int, float>>, em::Meta::TypeList<int, float>>);
static_assert(std::is_same_v<em::Meta::List::append_types<em::Meta::TypeList<int, float>, char, double>, em::Meta::TypeList<int, float, char, double>>);
static_assert(std::is_same_v<em::Meta::List::append_values<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::append_values<em::Meta::ValueList<>, 10, 20>, em::Meta::ValueList<10, 20>>);
static_assert(std::is_same_v<em::Meta::List::append_values<em::Meta::ValueList<10, 20>>, em::Meta::ValueList<10, 20>>);
static_assert(std::is_same_v<em::Meta::List::append_values<em::Meta::ValueList<10, 20>, 30, 40>, em::Meta::ValueList<10, 20, 30, 40>>);


// --- cat, cat_types, cat_values:

static_assert(std::is_same_v<em::Meta::List::cat_types<>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::cat_values<>, em::Meta::ValueList<>>);

static_assert(std::is_same_v<em::Meta::List::cat_types<em::Meta::TypeList<int>, em::Meta::TypeList<>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, char, float>>);
static_assert(std::is_same_v<em::Meta::List::cat      <em::Meta::TypeList<int>, em::Meta::TypeList<>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, char, float>>);
static_assert(std::is_same_v<em::Meta::List::cat_values<em::Meta::ValueList<10>, em::Meta::ValueList<>, em::Meta::ValueList<20, 30>>, em::Meta::ValueList<10, 20, 30>>);
static_assert(std::is_same_v<em::Meta::List::cat       <em::Meta::ValueList<10>, em::Meta::ValueList<>, em::Meta::ValueList<20, 30>>, em::Meta::ValueList<10, 20, 30>>);


// --- empty_of_same_kind:

static_assert(std::is_same_v<em::Meta::List::empty_of_same_kind<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::empty_of_same_kind<em::Meta::TypeList<int, float>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::empty_of_same_kind<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::empty_of_same_kind<em::Meta::ValueList<10, 20>>, em::Meta::ValueList<>>);


// --- type_at, value_at:

static_assert(std::is_same_v<em::Meta::List::type_at<em::Meta::TypeList<int, float, double>, 0>, int>);
static_assert(std::is_same_v<em::Meta::List::type_at<em::Meta::TypeList<int, float, double>, 1>, float>);
static_assert(std::is_same_v<em::Meta::List::type_at<em::Meta::TypeList<int, float, double>, 2>, double>);

static_assert(em::Meta::List::value_at<em::Meta::ValueList<10, 20, 30>, 0> == 10);
static_assert(em::Meta::List::value_at<em::Meta::ValueList<10, 20, 30>, 1> == 20);
static_assert(em::Meta::List::value_at<em::Meta::ValueList<10, 20, 30>, 2> == 30);


// --- contains_type, contains_value:

static_assert(em::Meta::List::contains_type<em::Meta::TypeList<int, float, double>, int> == true);
static_assert(em::Meta::List::contains_type<em::Meta::TypeList<int, float, double>, float> == true);
static_assert(em::Meta::List::contains_type<em::Meta::TypeList<int, float, double>, double> == true);
static_assert(em::Meta::List::contains_type<em::Meta::TypeList<int, float, double>, char> == false);
static_assert(em::Meta::List::contains_type<em::Meta::TypeList<>, char> == false);

static_assert(em::Meta::List::contains_value<em::Meta::ValueList<10, 20, 30>, 10> == true);
static_assert(em::Meta::List::contains_value<em::Meta::ValueList<10, 20, 30>, 20> == true);
static_assert(em::Meta::List::contains_value<em::Meta::ValueList<10, 20, 30>, 30> == true);
static_assert(em::Meta::List::contains_value<em::Meta::ValueList<10, 20, 30>, 40> == false);
static_assert(em::Meta::List::contains_value<em::Meta::ValueList<>, 40> == false);


// --- is_subset_of:

static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<>, em::Meta::TypeList<>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<>, em::Meta::TypeList<int, float, double>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<double, float>, em::Meta::TypeList<int, float, double>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<double, float, float>, em::Meta::TypeList<int, float, double>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<double, float>, em::Meta::TypeList<int, float, double, float>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<double, float, int>, em::Meta::TypeList<int, float, double>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, float, double>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, double>> == false);
static_assert(em::Meta::List::is_subset_of<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<>> == false);

static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<>, em::Meta::ValueList<>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<>, em::Meta::ValueList<10, 20, 30>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<30, 20>, em::Meta::ValueList<10, 20, 30>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<30, 20, 20>, em::Meta::ValueList<10, 20, 30>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<30, 20>, em::Meta::ValueList<10, 20, 30, 20>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<30, 20, 10>, em::Meta::ValueList<10, 20, 30>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<10, 20, 30>> == true);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<10, 30>> == false);
static_assert(em::Meta::List::is_subset_of<em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<>> == false);


// --- have_same_elems:

static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<>, em::Meta::TypeList<>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<int>, em::Meta::TypeList<>> == false);
static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<>, em::Meta::TypeList<int>> == false);
static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<int>, em::Meta::TypeList<int>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<int, float, char>, em::Meta::TypeList<float, char, int>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::TypeList<int, float, char, float, int, float>, em::Meta::TypeList<float, char, int, char>> == true);

static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<>, em::Meta::ValueList<>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<10>, em::Meta::ValueList<>> == false);
static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<>, em::Meta::ValueList<10>> == false);
static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<10>, em::Meta::ValueList<10>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<20, 30, 10>> == true);
static_assert(em::Meta::List::have_same_elems<em::Meta::ValueList<10, 20, 30, 20, 10, 20>, em::Meta::ValueList<20, 30, 10, 30>> == true);


// --- have_same_elems_and_size:

static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<>, em::Meta::TypeList<>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<int>, em::Meta::TypeList<>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<>, em::Meta::TypeList<int>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<int>, em::Meta::TypeList<int>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<int, float, char>, em::Meta::TypeList<float, char, int>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<int, float, char, float, int, float>, em::Meta::TypeList<float, char, int, char>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::TypeList<int, float, char, float>, em::Meta::TypeList<float, char, int, char>> == true);

static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<>, em::Meta::ValueList<>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<10>, em::Meta::ValueList<>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<>, em::Meta::ValueList<10>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<10>, em::Meta::ValueList<10>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<20, 30, 10>> == true);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<10, 20, 30, 20, 10, 20>, em::Meta::ValueList<20, 30, 10, 30>> == false);
static_assert(em::Meta::List::have_same_elems_and_size<em::Meta::ValueList<10, 20, 30, 20>, em::Meta::ValueList<20, 30, 10, 30>> == true);


// --- find_type, find_value:

using F1 = em::Meta::List::find_type<em::Meta::TypeList<>, int>;
static_assert(F1::found == false);
static_assert(std::is_same_v<F1::remaining, em::Meta::TypeList<>>);
static_assert(F1::value == 0);

using F2 = em::Meta::List::find_type<em::Meta::TypeList<char, float, int, float, int, double>, int>;
static_assert(F2::found == true);
static_assert(std::is_same_v<F2::remaining, em::Meta::TypeList<float, int, double>>);
static_assert(F2::value == 2);

using F3 = em::Meta::List::find_type<em::Meta::TypeList<char, float, int, float, int, double>, short>;
static_assert(F3::found == false);
static_assert(std::is_same_v<F3::remaining, em::Meta::TypeList<>>);
static_assert(F3::value == 6);

using F4 = em::Meta::List::find_value<em::Meta::ValueList<>, 10>;
static_assert(F4::found == false);
static_assert(std::is_same_v<F4::remaining, em::Meta::ValueList<>>);
static_assert(F4::value == 0);

using F5 = em::Meta::List::find_value<em::Meta::ValueList<20, 30, 10, 30, 10, 40>, 10>;
static_assert(F5::found == true);
static_assert(std::is_same_v<F5::remaining, em::Meta::ValueList<30, 10, 40>>);
static_assert(F5::value == 2);

using F6 = em::Meta::List::find_value<em::Meta::ValueList<20, 30, 10, 30, 10, 40>, 50>;
static_assert(F6::found == false);
static_assert(std::is_same_v<F6::remaining, em::Meta::ValueList<>>);
static_assert(F6::value == 6);


// --- copy_uniq:

static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::TypeList<>, em::Meta::TypeList<int, float, int>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::TypeList<int, float, int>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::TypeList<int, float, int, short>, em::Meta::TypeList<double, int, char>>, em::Meta::TypeList<double, int, char, float, short>>);

static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::ValueList<>, em::Meta::ValueList<10, 20, 10>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<>>, em::Meta::ValueList<10, 20>>);
static_assert(std::is_same_v<em::Meta::List::copy_uniq<em::Meta::ValueList<10, 20, 10, 30>, em::Meta::ValueList<40, 10, 50>>, em::Meta::ValueList<40, 10, 50, 20, 30>>);


// --- uniq:

static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::TypeList<int>>, em::Meta::TypeList<int>>);
static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::TypeList<int, short, int, char, float, float>>, em::Meta::TypeList<int, short, char, float>>);

static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::ValueList<10>>, em::Meta::ValueList<10>>);
static_assert(std::is_same_v<em::Meta::List::uniq<em::Meta::ValueList<10, 20, 10, 30, 40, 40>>, em::Meta::ValueList<10, 20, 30, 40>>);


// --- copy_subtract:

static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<>, em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<>, em::Meta::TypeList<int, float, int>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<>, em::Meta::TypeList<int, float, int>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<>, em::Meta::TypeList<int, float, int>, em::Meta::TypeList<char>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<int, float, int>, em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<int, float, int>, em::Meta::TypeList<int, float, char>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, char, int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<int, float, char, int>, em::Meta::TypeList<int, float, char>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, float, char, int, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<int, float, char, int>, em::Meta::TypeList<int, float, char>, em::Meta::TypeList<>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, float, char, int, int>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::TypeList<int, float, char, int>, em::Meta::TypeList<int, float, char>, em::Meta::TypeList<float>, em::Meta::TypeList<char>>, em::Meta::TypeList<int, float, char, int, int>>);

static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<>, em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<>, em::Meta::ValueList<10, 20, 10>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<>, em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<>, em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<30>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<>>, em::Meta::ValueList<10, 20, 30, 10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<10, 20, 30, 10>, em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<30, 20>>, em::Meta::ValueList<10, 20, 30, 10, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<10, 20, 30, 10>, em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<>, em::Meta::ValueList<30, 20>>, em::Meta::ValueList<10, 20, 30, 10, 10>>);
static_assert(std::is_same_v<em::Meta::List::copy_subtract<em::Meta::ValueList<10, 20, 30, 10>, em::Meta::ValueList<10, 20, 30>, em::Meta::ValueList<20>, em::Meta::ValueList<30>>, em::Meta::ValueList<10, 20, 30, 10, 10>>);


// --- subtract:

static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<>, em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<int, float, int>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, int>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<int, float, int, char, short>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, int, short>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<int, float, int, char, short>, em::Meta::TypeList<>, em::Meta::TypeList<char, float>>, em::Meta::TypeList<int, int, short>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::TypeList<int, float, int, char, short>, em::Meta::TypeList<float>, em::Meta::TypeList<char>>, em::Meta::TypeList<int, int, short>>);

static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<>, em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<10, 20, 10>, em::Meta::ValueList<>>, em::Meta::ValueList<10, 20, 10>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<10, 20, 10, 30, 40>, em::Meta::ValueList<30, 20>>, em::Meta::ValueList<10, 10, 40>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<10, 20, 10, 30, 40>, em::Meta::ValueList<>, em::Meta::ValueList<30, 20>>, em::Meta::ValueList<10, 10, 40>>);
static_assert(std::is_same_v<em::Meta::List::subtract<em::Meta::ValueList<10, 20, 10, 30, 40>, em::Meta::ValueList<20>, em::Meta::ValueList<30>>, em::Meta::ValueList<10, 10, 40>>);


// --- subtract_ordered:

// types:
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, double>>, em::Meta::TypeList<float>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<float, int>>, em::Meta::TypeList<int, double>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<float, int>, em::Meta::TypeList<double>>, em::Meta::TypeList<int>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<int, float>, em::Meta::TypeList<double>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>, em::Meta::TypeList<>>, em::Meta::TypeList<int, float, double>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<int, float, double>>, em::Meta::TypeList<int, float, double>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<>, em::Meta::TypeList<int, float>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::TypeList<>, em::Meta::TypeList<>>, em::Meta::TypeList<>>);
// values:
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<1, 3>>, em::Meta::ValueList<2>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<2, 1>>, em::Meta::ValueList<1, 3>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<2, 1>, em::Meta::ValueList<3>>, em::Meta::ValueList<1>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<1, 2>, em::Meta::ValueList<3>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>, em::Meta::ValueList<>>, em::Meta::ValueList<1, 2, 3>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<1, 2, 3>>, em::Meta::ValueList<1, 2, 3>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<>, em::Meta::ValueList<1, 2>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::subtract_ordered<em::Meta::ValueList<>, em::Meta::ValueList<>>, em::Meta::ValueList<>>);


// --- reverse:
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::TypeList<>>, em::Meta::TypeList<>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::ValueList<>>, em::Meta::ValueList<>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::TypeList<int, float, double>>, em::Meta::TypeList<double, float, int>>);
static_assert(std::is_same_v<em::Meta::List::reverse<em::Meta::ValueList<10, 20, 30>>, em::Meta::ValueList<30, 20, 10>>);


// --- split_types, split_values:

struct Small {template <typename T> using type = std::bool_constant<sizeof(T) < 4>;};
struct SmallValues {template <auto X> using type = std::bool_constant<X < 4>;};

static_assert(std::is_same_v<em::Meta::List::split_types<Small, em::Meta::TypeList<>>, em::Meta::TypePair<em::Meta::TypeList<>, em::Meta::TypeList<>>>);
static_assert(std::is_same_v<em::Meta::List::split_types<Small, em::Meta::TypeList<int, float>>, em::Meta::TypePair<em::Meta::TypeList<>, em::Meta::TypeList<int, float>>>);
static_assert(std::is_same_v<em::Meta::List::split_types<Small, em::Meta::TypeList<char, short>>, em::Meta::TypePair<em::Meta::TypeList<char, short>, em::Meta::TypeList<>>>);
static_assert(std::is_same_v<em::Meta::List::split_types<Small, em::Meta::TypeList<char, int, float, short>>, em::Meta::TypePair<em::Meta::TypeList<char, short>, em::Meta::TypeList<int, float>>>);

static_assert(std::is_same_v<em::Meta::List::split_values<SmallValues, em::Meta::ValueList<>>, em::Meta::TypePair<em::Meta::ValueList<>, em::Meta::ValueList<>>>);
static_assert(std::is_same_v<em::Meta::List::split_values<SmallValues, em::Meta::ValueList<4, 5>>, em::Meta::TypePair<em::Meta::ValueList<>, em::Meta::ValueList<4, 5>>>);
static_assert(std::is_same_v<em::Meta::List::split_values<SmallValues, em::Meta::ValueList<2, 3>>, em::Meta::TypePair<em::Meta::ValueList<2, 3>, em::Meta::ValueList<>>>);
static_assert(std::is_same_v<em::Meta::List::split_values<SmallValues, em::Meta::ValueList<2, 4, 5, 3>>, em::Meta::TypePair<em::Meta::ValueList<2, 3>, em::Meta::ValueList<4, 5>>>);
