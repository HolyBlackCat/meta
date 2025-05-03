#pragma once

#include "em/meta/cvref.h"
#include "em/meta/deduce.h"

#include <type_traits>

namespace em::Meta
{
    // Copies cvref-qualifiers from the source type to the target, then casts to that.
    // Intentionally casts through a pointer to avoid calling user-defined conversion operators.
    template <Meta::cvref_unqualified To, Meta::Deduce..., typename From>
    [[nodiscard]] constexpr auto static_cast_to_cvref(From &&source) -> Meta::copy_cvref<From &&, To>
    requires requires{static_cast<std::remove_reference_t<To> *>(&source);}
    {
        return static_cast<Meta::copy_cvref<From &&, To>>(*static_cast<std::remove_reference_t<To> *>(&source));
    }
}
