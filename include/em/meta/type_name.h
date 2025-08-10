#pragma once

// This is a thin wrapper over `cppdecl::TypeName()` that avoid the inclusion of cppdecl in debug builds, at the cost of losing constexpr-ness.

// Only enable constexpr type names in release builds, because of the slow compilation speeds.
#ifdef NDEBUG
#define EM_CONSTEXPR_TYPE_NAME 1
#else
#define EM_CONSTEXPR_TYPE_NAME 0
#endif

#include "em/zstring_view.h"

#if EM_CONSTEXPR_TYPE_NAME
#include <cppdecl/type_name.h>
#endif

#include <string>
#include <typeindex>
#include <typeinfo> // IWYU pragma: keep. We do use `typeid()` below, silence the false positive.

namespace em::Meta
{
    [[nodiscard]] std::string TypeNameDynamic(std::type_index type);

    template <typename T>
    [[nodiscard]] zstring_view TypeName()
    {
        #if EM_CONSTEXPR_TYPE_NAME
        return zstring_view(zstring_view::TrustNullTerminated{}, cppdecl::TypeName<T>());
        #else
        static const std::string ret = TypeNameDynamic(typeid(T));
        return ret;
        #endif
    }
}
