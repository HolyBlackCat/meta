#pragma once

namespace em::Meta
{
    // This can be used as `template <__, None..., __>`, to force the template parameters after it to be deduced.
    // Don't rely on this being a concept, only use the exact syntax given above.
    template <typename> concept Deduce = false;
}
