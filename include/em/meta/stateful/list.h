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
                    return Tag<Value>{};
                }
            };

            constexpr void _adl_em_StatefulListElem() {} // A dummy ADL target.

            // We have two different strategies to compute the list size, a linear search and an exponential search.
            // The latter is faster on very large lists, but could in theory have more overhead on many small lists.
            #ifndef EM_STATEFUL_LIST_USE_EXPONENTIAL_SEARCH
            #define EM_STATEFUL_LIST_USE_EXPONENTIAL_SEARCH 1
            #endif
            #if !EM_STATEFUL_LIST_USE_EXPONENTIAL_SEARCH // Naive linear search
            template <typename Name, typename Unique, std::size_t Index = 0, typename = void>
            struct CalcSize : std::integral_constant<std::size_t, Index> {};
            template <typename Name, typename Unique, std::size_t Index>
            struct CalcSize<Name, Unique, Index, decltype(void(_adl_em_StatefulListElem(ElemReader<Name, Index>{})))> : CalcSize<Name, Unique, Index + 1> {};
            #else // Exponential search.
            // See below.
            template <typename Name, typename Unique, std::size_t Low, std::size_t High>
            struct CalcSize1;

            // Checks if `Mid` is a valid index or not.
            // Calls `CalcSize1` with the correct half of the range.
            template <typename Name, typename Unique, std::size_t Low, std::size_t High, std::size_t Mid, typename = void>
            struct CalcSize2 : CalcSize1<Name, Unique, Low, Mid> {};
            template <typename Name, typename Unique, std::size_t Low, std::size_t High, std::size_t Mid>
            struct CalcSize2<Name, Unique, Low, High, Mid, decltype(void(_adl_em_StatefulListElem(ElemReader<Name, Mid>{})))> : CalcSize1<Name, Unique, Mid, High> {};

            // If the range has size 1 or 0, returns the end of the range. Otherwise passes it to `CalcSize2`.
            // The size range 0 is only possible if the entire range we're operating on is empty.
            template <typename Name, typename Unique, std::size_t Low, std::size_t High>
            struct CalcSize1 : CalcSize2<Name, Unique, Low, High, (Low + High) / 2> {};
            template <typename Name, typename Unique, std::size_t Low>
            struct CalcSize1<Name, Unique, Low, Low> : std::integral_constant<std::size_t, Low> {};
            template <typename Name, typename Unique, std::size_t Low>
            struct CalcSize1<Name, Unique, Low, Low + 1> : std::integral_constant<std::size_t, Low + 1> {};

            // Checks the powers of two.
            // When we find one that's not a valid index, passes the two latest numbers to `CalcSize1`.
            template <typename Name, typename Unique, std::size_t Low = 0, std::size_t High = 0, typename = void>
            struct CalcSize : CalcSize1<Name, Unique, Low, High> {};
            template <typename Name, typename Unique, std::size_t Low, std::size_t High>
            struct CalcSize<Name, Unique, Low, High, decltype(void(_adl_em_StatefulListElem(ElemReader<Name, High>{})))> : CalcSize<Name, Unique, High, High ? High * 2 : 1> {};
            #endif

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
        inline constexpr std::size_t size = detail::CalcSize<Name, Unique>::value;

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
