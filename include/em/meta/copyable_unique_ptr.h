#pragma once

#include <concepts>
#include <memory>

namespace em::Meta
{
    // A copyable wrapper for `std::unique_ptr`.
    // Only works correctly when inheritance is not involved.
    template <typename T>
    struct CopyableUniquePtr : std::unique_ptr<T>
    {
        using std::unique_ptr<T>::unique_ptr;

        constexpr CopyableUniquePtr(const CopyableUniquePtr<T> &other) : CopyableUniquePtr(static_cast<const std::unique_ptr<T> &>(other)) {}
        constexpr CopyableUniquePtr(CopyableUniquePtr<T> &&other) noexcept : CopyableUniquePtr(static_cast<std::unique_ptr<T> &&>(other)) {}
        constexpr CopyableUniquePtr &operator=(const CopyableUniquePtr<T> &other) {return *this = static_cast<const std::unique_ptr<T> &>(other);}
        constexpr CopyableUniquePtr &operator=(CopyableUniquePtr<T> &&other) noexcept {return *this = static_cast<std::unique_ptr<T> &&>(other);}

        constexpr CopyableUniquePtr(const std::unique_ptr<T> &other) : std::unique_ptr<T>(other ? std::make_unique<T>(*other) : nullptr) {}
        constexpr CopyableUniquePtr(std::unique_ptr<T> &&other) noexcept : std::unique_ptr<T>(std::move(other)) {}
        constexpr CopyableUniquePtr &operator=(const std::unique_ptr<T> &other)
        {
            std::unique_ptr<T>::operator=(other ? std::make_unique<T>(*other) : nullptr);
            return *this;
        }
        constexpr CopyableUniquePtr &operator=(std::unique_ptr<T> &&other) noexcept
        {
            std::unique_ptr<T>::operator=(std::move(other));
            return *this;
        }

        // Reject `unique_ptr`s to derived classes, because we wouldn't be able to copy them properly.
        template <std::derived_from<T> U> CopyableUniquePtr(const std::unique_ptr<U> &) = delete;
        template <std::derived_from<T> U> CopyableUniquePtr(std::unique_ptr<U> &&) = delete;
        template <std::derived_from<T> U> CopyableUniquePtr &operator=(const std::unique_ptr<U> &) = delete;
        template <std::derived_from<T> U> CopyableUniquePtr &operator=(std::unique_ptr<U> &&) = delete;

        // Need this, otherwise assigning nullptr is ambiguous.
        constexpr CopyableUniquePtr &operator=(std::nullptr_t)
        {
            std::unique_ptr<T>::operator=(nullptr);
            return *this;
        }
    };
}
