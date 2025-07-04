#pragma once

#include "em/macros/utils/forward.h"
#include "em/meta/common.h"

#include <type_traits>

namespace em::Meta
{
    // Copies cvref-qualifiers from the source type to the target, then casts to that.
    // Intentionally casts through a pointer to avoid calling user-defined conversion operators.
    template <Meta::cvref_unqualified To, Meta::Deduce..., typename From>
    [[nodiscard]] constexpr auto static_cast_to_cvref(From &&source) noexcept -> Meta::copy_cvref<From &&, To>
    requires requires{static_cast<To *>(&source);}
    {
        return static_cast<Meta::copy_cvref<From &&, To>>(*static_cast<To *>(&source));
    }

    // Casts the input to `To`.
    // If it's already of the same type (or derived from it), instead returns
    //   a reference to the subobject of that type (with the original cvref-qualifiers preserved).
    template <Meta::cvref_unqualified To, Meta::Deduce..., typename From, bool PtrConv = std::is_convertible_v<const std::remove_cvref_t<From> *, const To *>>
    [[nodiscard]] constexpr auto static_cast_or_return_ref(From &&source) noexcept -> std::conditional_t<PtrConv, copy_cvref<From &&, To>, To>
    requires PtrConv || std::is_constructible_v<To, From &&>
    {
        if constexpr (PtrConv)
            return static_cast<copy_cvref<From &&, To>>(*static_cast<copy_cv<From, To> *>(&source));
        else
            return static_cast<To>(EM_FWD(source));
    }
}
