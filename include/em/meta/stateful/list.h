#pragma once

#include "em/macros/portable/warnings.h"
#include "em/meta/lists.h"
#include "em/meta/tags.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace em::Meta::Stateful
{
    namespace List
    {
        namespace detail
        {
            template <typename Name, std::size_t Index>
            struct ElemReader
            {
                EM_SILENCE_NON_TEMPLATE_FRIEND(
                friend constexpr auto _adl_em_StatefulListElem(ElemReader<Name, Index>);
                )
            };

            template <typename Name, std::size_t Index, typename Value>
            struct ElemWriter
            {
                friend constexpr auto _adl_em_StatefulListElem(ElemReader<Name, Index>)
                {
                    return tag<Value>{};
                }
            };

            constexpr void _adl_em_StatefulListElem() {} // A dummy ADL target.

            template <typename Name, std::size_t Index, typename Unique, typename = void>
            struct CalcSize : std::integral_constant<std::size_t, Index> {};

            template <typename Name, std::size_t Index, typename Unique>
            struct CalcSize<Name, Index, Unique, decltype(void(_adl_em_StatefulListElem(ElemReader<Name, Index>{})))> : CalcSize<Name, Index + 1, Unique> {};

            template <typename Name, std::size_t Index, typename Unique>
            using ReadElem = typename decltype(_adl_em_StatefulListElem(ElemReader<Name, Index>{}))::type;

            template <typename Name, typename I, typename Unique>
            struct ReadElemList {};
            template <typename Name, std::size_t ...I, typename Unique>
            struct ReadElemList<Name, std::index_sequence<I...>, Unique> {using type = TypeList<ReadElem<Name, I, Unique>...>;};
        }

        struct DefaultUnique {};

        template <typename T>
        struct DefaultPushBackUnique {};

        // Calculates the current list size.
        template <typename Name, typename Unique = DefaultUnique>
        inline constexpr std::size_t size = detail::CalcSize<Name, 0, Unique>::value;

        // Touch this type to append `Value` to the list.
        template <typename Name, typename Value, typename Unique = DefaultPushBackUnique<Value>>
        using PushBack = detail::ElemWriter<Name, size<Name, Unique>, Value>;

        // Returns the type previously passed to `WriteState`, or causes a SFINAE error.
        template <typename Name, std::size_t I, typename Unique = DefaultUnique>
        using Elem = detail::ReadElem<Name, I, Unique>;

        // Returns the list elements as `Meta::TypeList<...>`.
        template <typename Name, typename Unique = DefaultUnique>
        using Elems = typename detail::ReadElemList<Name, std::make_index_sequence<size<Name, Unique>>, Unique>::type;
    }
}
