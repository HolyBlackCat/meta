#pragma once

// Helpers for comparisons, primarily three-way.

namespace em::Meta
{
    // A wrapper for `std::..._ordering` that has an `operator bool`.
    template <typename T>
    struct BoolTestableCompareResult
    {
        T value{};

        // Returns true if not `equivalent`.
        [[nodiscard]] constexpr explicit operator bool() const noexcept
        {
            // Spelling it like this allows `T` to be `int`, or something. Not sure why would anyone do that.
            return value != 0;
        }
    };
}
