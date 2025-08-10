#include "em/meta/type_name.h"

#include <cppdecl/type_name.h>

namespace em::Meta
{
    std::string TypeNameDynamic(std::type_index type)
    {
        return cppdecl::TypeNameDynamic(type);
    }
}
