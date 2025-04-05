#pragma once

namespace em::Meta
{
    namespace detail
    {
        template <template <typename, typename> typename T, typename P0, typename ...P>
        struct Reduce {};

        template <template <typename, typename> typename T, typename P0>
        struct Reduce<T, P0> {using type = P0;};

        template <template <typename, typename> typename T, typename P0, typename P1, typename ...P>
        requires requires{typename T<P0, typename Reduce<T, P1, P...>::type>;}
        struct Reduce<T, P0, P1, P...> {using type = T<P0, typename Reduce<T, P1, P...>::type>;};


        template <template <typename, typename> typename T, typename P0, typename ...P>
        struct ReduceIndirect {};

        template <template <typename, typename> typename T, typename P0>
        struct ReduceIndirect<T, P0> {using type = P0;};

        template <template <typename, typename> typename T, typename P0, typename P1, typename ...P>
        requires requires{typename T<P0, typename ReduceIndirect<T, P1, P...>::type>::type;}
        struct ReduceIndirect<T, P0, P1, P...> {using type = typename T<P0, typename ReduceIndirect<T, P1, P...>::type>::type;};
    }

    // Reduces list `P...` over `T<A,B>`. If there's only one `P`, returns it unchanged. If `P...` is empty, fails.
    template <template <typename, typename> typename T, typename ...P>
    requires requires{typename detail::Reduce<T, P...>::type;}
    using reduce_types = detail::Reduce<T, P...>::type;

    // Reduces list `P...` over `T<A,B>::type`. If there's only one `P`, returns it unchanged. If `P...` is empty, fails.
    template <template <typename, typename> typename T, typename ...P>
    requires requires{typename detail::ReduceIndirect<T, P...>::type;}
    using reduce_types_indirect = detail::ReduceIndirect<T, P...>::type;
}
