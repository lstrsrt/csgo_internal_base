#pragma once

#include <cstddef>
#include <cstdint>

class address {
public:
    address() = default;
    address(uintptr_t value) noexcept
        : value(value) {};
    address(uint8_t* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) {};
    address(uintptr_t* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) {};
    address(void* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) {};

    constexpr operator bool() const noexcept
    {
        return value != 0;
    }

    operator void*() const noexcept
    {
        return reinterpret_cast<void*>(value);
    }

    template<class ty>
    constexpr ty cast() noexcept
    {
        return reinterpret_cast<ty>(value);
    }

    constexpr address& offset(ptrdiff_t offset) noexcept
    {
        value += offset;
        return *this;
    }

    address& dereference() noexcept
    {
        return *reinterpret_cast<address*>(value);
    }

    template<class ty>
    ty absolute(ptrdiff_t offset = 0) noexcept
    {
        const auto disp = *reinterpret_cast<int32_t*>(value);
        if (disp)
            // Base address + offset to jmp instruction + size of next instruction + target address.
            return reinterpret_cast<ty>(value + offset + 4 + disp);
        return ty();
    }

private:
    uintptr_t value{ };
};
