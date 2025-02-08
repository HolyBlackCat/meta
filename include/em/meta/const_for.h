#pragma once

#include "em/macros/meta/eval.h"
#include "em/macros/utils/forward.h"
#include "em/macros/utils/if_maybe_constexpr.h"
#include "em/macros/utils/lift.h"
#include "em/meta/deduce.h"
#include "em/meta/lists.h"
#include "em/meta/void.h"

#include <utility>

namespace em::Meta
{
    namespace detail
    {
        EM_EVAL(
            EM_FOREACH_A( (typename)(auto) )
            (
                template <EM_A I0, EM_A ...I>
                constexpr decltype(auto) ConstForAdvanced(auto &&func)
                {
                    decltype(auto) ret = InvokeWithVoidPlaceholderResult(EM_EXPR(func.template operator()<I0>()));

                    if constexpr (sizeof...(I) == 0)
                    {
                        EM_RETURN_VARIABLE(ret);
                    }
                    else
                    {
                        EM_IF_ELSE_MAYBE_CONSTEXPR(
                            VoidToFalse(ret),
                            (
                                EM_RETURN_VARIABLE(ret);
                            ),
                            (
                                return (ConstForAdvanced<I...>)(EM_FWD(func));
                            )
                        )
                    }
                }
            )
        )
    }

    // An advanced constexpr `for` loop.
    // `func` is `[]<Type I>{...}`, calls it for `0..N-1`.
    // If `func` returns non-void and truthy, stops immediately and returns that result.
    // Otherwise runs all iterations and returns the result of the last iteration, or void if no iterations.
    // If the result of `func()` is constexpr-convertible to `bool`, the return type can depend on `I`. Otherwise it can't.
    //
    // So e.g. you can do `if constexpr (...) return std::true_type{};` (else don't return at all), will optimally stop the loop.
    template <auto N, Meta::Deduce...>
    [[nodiscard]] constexpr decltype(auto) ConstForAdvanced(auto &&func)
    {
        if constexpr (N > 0)
        {
            return [&]<decltype(N) ...I>(std::integer_sequence<decltype(N), I...>) -> decltype(auto)
            {
                return detail::ConstForAdvanced<I...>(EM_FWD(func));
            }(std::make_integer_sequence<decltype(N), N>{});
        }
    }

    // Same but for-each over a type list.
    template <typename ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachAdvanced(TypeList<T...>, auto &&func)
    {
        if constexpr (sizeof...(T))
        return detail::ConstForAdvanced<T...>(EM_FWD(func));
    }
    // Same but for-each over a value list.
    template <auto ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachAdvanced(ValueList<T...>, auto &&func)
    {
        if constexpr (sizeof...(T))
            return detail::ConstForAdvanced<T...>(EM_FWD(func));
    }

    // Same but for-each over a type pack.
    template <typename ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachAdvanced(auto &&func)
    {
        if constexpr (sizeof...(T))
        return detail::ConstForAdvanced<T...>(EM_FWD(func));
    }
    // Same but for-each over a value pack. (this overload rejects empty packs, but they are handled by the previous overload)
    template <auto T0, auto ...T>
    [[nodiscard]] constexpr decltype(auto) ConstForEachAdvanced(auto &&func)
    {
        return detail::ConstForAdvanced<T0, T...>(EM_FWD(func));
    }
}
