#include "em/meta/cvref.h"
#include "em/meta/stateful/list.h"

// This file must be used together with `em/macros/meta/detectable_base.h`. That file is in a different repository.
//
// Use `EM_DETECTABLE_BASE(...)` from that file to mark your base classes, and use this file to detect them.
//
// The bases can be obtained from `em::Meta::DetectBases::{All,Virtual,NonVirtual}Bases{Flat,Direct}{,AndSelf}`, which return `em::Meta::TypeList<...>`.
// `Flat` recursively returns bases of bases too. `AndSelf` includes the self type at the end of the list.
// Not all combinations are implemented: `Direct` is only implemented for `NonVirtual`. It doesn't make much sense for other variants anyway.
//
// Most often you'll be using it like this: first iterate over `VirtualBasesFlatAndSelf`, then recursively iterate over `NonVirtualBasesDirect`.
//
// LIMITATIONS:
//   1. If the same base appears as both virtual and non-virtual (both possibly nested) in the same class, then the virtual one will not be detected.
//        This will get fixed when `std::is_virtual_base_of` becomes available and we switch to it. It's a limitation of the handwritten replacement trait.
//   2. If an ambiguous base is both a direct and an indirect base, then the direct copy will get skipped.
//        That direct base is completely inaccessible anyway, but it's a bit annoying that we don't detect skipping it.
//        At least Clang warns about it (regardless of our reflection).

namespace em::Meta::DetectBases
{
    namespace detail
    {
        template <typename Tag, typename T, template <typename Tag_, typename Base, typename Derived> typename Pred>
        constexpr void _adl_em_DetectBase(void *) {} // A dummy ADL target.

        // Populates a stateful list with bases of `T`.
        // `Tag` is whatever tag was passed to `EM_DETECTABLE_BASE()`.
        // `SelectListTag` must return a tag of a stateful list that we're filling. Or it can SFINAE-fail to skip this base.
        // `SelectListTag` MUST check that `Base` is a base of `Derived`, otherwise you can get various junk into the list.
        // `SelectListTag` receives the same `Tag_` as you pass into `Tag`.
        template <typename Tag, typename T, template <typename Tag_, typename Base, typename Derived> typename SelectListTag>
        constexpr auto Detect() -> decltype(void(_adl_em_DetectBase<Tag, T, SelectListTag>((T *)nullptr))) {}

        // Returns true if `Base` is a virtual base of `Derived`. (False if both types are the same.)
        // Inspired by this implementation: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2985r0.html#usercode
        // TODO: Migrate this to `std::is_virtual_base_of` when that becomes available. Note that it will automatically reject the same type,
        //   which here we have to do manually.
        // This currently returns false for ambiguous virtual bases, while `std::is_virtual_base_of` will return true, which will be better.
        //   But this does handle inaccessible virtual bases correctly, because C-style casts can handle that.
        template <typename Base, typename Derived>
        concept IsVirtualBase =
            !Meta::same_ignoring_cv<Base, Derived> &&
            requires(const volatile Derived *d){(const volatile Base *)d;} &&
            !requires(const volatile Base *b){(const volatile Derived *)b;};

        // All bases recursively, but not self.
        template <typename Tag, typename Derived>
        struct TagAllBases {};
        template <typename Tag, typename Base, typename Derived> requires (!std::is_same_v<Base, Derived>) && std::is_base_of_v<Base, Derived>
        using SelectTagAllBases = TagAllBases<Tag, Derived>;

        // All virtual bases recursively, but not self.
        template <typename Tag, typename Derived>
        struct TagVirtualBases {};
        template <typename Tag, typename Base, typename Derived> requires IsVirtualBase<Base, Derived>
        using SelectTagVirtualBases = TagVirtualBases<Tag, Derived>;

        // All non-virtual bases recursively, but not self.
        template <typename Tag, typename Derived>
        struct TagNonVirtualBases {};
        template <typename Tag, typename Base, typename Derived> requires (!std::is_same_v<Base, Derived>) && std::is_base_of_v<Base, Derived> && (!IsVirtualBase<Base, Derived>)
        using SelectTagNonVirtualBases = TagNonVirtualBases<Tag, Derived>;
    }

    // A flat list of all bases, including indirect ones, excluding self. Never contains repetitions.
    template <typename Tag, typename T>
    using AllBasesFlat = decltype((detail::Detect<Tag, T, detail::SelectTagAllBases>()), Meta::Stateful::List::Elems<detail::TagAllBases<Tag, T>>{});
    // Same, but includes `T` as the last type in the list.
    template <typename Tag, typename T>
    using AllBasesFlatAndSelf = Meta::list_append_types<AllBasesFlat<Tag, T>, T>;

    // A flat list of all virtual bases, including indirect ones, excluding self. Naturally never contains repetitions.
    template <typename Tag, typename T>
    using VirtualBasesFlat = decltype((detail::Detect<Tag, T, detail::SelectTagVirtualBases>()), Meta::Stateful::List::Elems<detail::TagVirtualBases<Tag, T>>{});
    // Same, but includes `T` as the last type in the list.
    template <typename Tag, typename T>
    using VirtualBasesFlatAndSelf = Meta::list_append_types<VirtualBasesFlat<Tag, T>, T>;

    // A flat list of all non-virtual bases, including indirect ones, excluding self. Never contains repetitions.
    template <typename Tag, typename T>
    using NonVirtualBasesFlat = decltype((detail::Detect<Tag, T, detail::SelectTagNonVirtualBases>()), Meta::Stateful::List::Elems<detail::TagNonVirtualBases<Tag, T>>{});
    // Same, but includes `T` as the last type in the list.
    template <typename Tag, typename T>
    using NonVirtualBasesFlatAndSelf = Meta::list_append_types<NonVirtualBasesFlat<Tag, T>, T>;


    namespace detail
    {
        template <typename Tag, typename T, typename U = T> struct SubtractBases {};
        template <typename Tag, typename T, typename ...U> struct SubtractBases<Tag, T, Meta::TypeList<U...>> {using type = Meta::list_subtract<T, NonVirtualBasesFlat<Tag, U>...>;};
    }

    // Direct non-virtual bases, excluding self.
    template <typename Tag, typename T>
    using NonVirtualBasesDirect = typename detail::SubtractBases<Tag, NonVirtualBasesFlat<Tag, T>>::type;
}
