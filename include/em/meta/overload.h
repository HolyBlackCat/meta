#pragma once

namespace em::Meta
{
    template <typename ...P>
    struct Overload : P...
    {
        using P::operator()...;
    };

    template <typename ...P>
    Overload(P...) -> Overload<P...>;
}
