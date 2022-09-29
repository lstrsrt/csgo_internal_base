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
    ty absolute(ptrdiff_t rel_offset = 0x1, ptrdiff_t abs_offset = 0x0) noexcept
    {
        const auto target = *reinterpret_cast<int32_t*>(value + rel_offset);
        if (target)
            // Base address + offset to jmp instruction + size of next instruction + target address.
            return reinterpret_cast<ty>(value + abs_offset + 4 + target);
        return ty();
    }

private:
    uintptr_t value{ };
};
