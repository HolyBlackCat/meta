#include "em/meta/common.h"

static_assert(!em::Meta::possibly_string_literal_cvref<int>);
static_assert(!em::Meta::possibly_string_literal_cvref<char>);
static_assert(!em::Meta::possibly_string_literal_cvref<const char>);
static_assert(!em::Meta::possibly_string_literal_cvref<char[]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const char[]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const char (&)[]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const char (&&)[]>);
static_assert(!em::Meta::possibly_string_literal_cvref<char[42]>);
static_assert(em::Meta::possibly_string_literal_cvref<const char[42]>);
static_assert(em::Meta::possibly_string_literal_cvref<const char (&)[42]>);
static_assert(em::Meta::possibly_string_literal_cvref<const char (&&)[42]>);
static_assert(!em::Meta::possibly_string_literal_cvref<char (&)[42]>);
static_assert(!em::Meta::possibly_string_literal_cvref<char (&&)[42]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const unsigned char[42]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const signed char[42]>);
static_assert(!em::Meta::possibly_string_literal_cvref<const int[42]>);
