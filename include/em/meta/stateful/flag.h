#pragma once

#include "em/macros/portable/warnings.h"

#include <cstddef>

namespace em::Meta::Stateful::Flag
{
    namespace detail
    {
        template <typename Tag>
        struct Reader
        {
            EM_SILENCE_NON_TEMPLATE_FRIEND(
            friend constexpr auto _adl_em_StatefulFlag(Reader<Tag>);
            )
        };

        template <typename Tag>
        struct Writer
        {
            friend constexpr auto _adl_em_StatefulFlag(Reader<Tag>)
            {
                return nullptr;
            }
        };

        constexpr void _adl_em_StatefulFlag() {} // A dummy ADL target.

        template <typename Tag, typename Unique, typename = std::nullptr_t>
        constexpr bool value = false;
        template <typename Tag, typename Unique>
        constexpr bool value<Tag, Unique, decltype(_adl_em_StatefulFlag(Reader<Tag>{}))> = true;

    }

    struct DefaultUnique {};

    // Sets the flag to true.
    template <typename Tag>
    using Set = detail::Writer<Tag>;

    // Reads the value of the flag.
    template <typename Tag, typename Unique = DefaultUnique>
    constexpr bool value = detail::value<Tag, Unique>;
}
