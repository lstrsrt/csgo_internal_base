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

    template<class ty>
    ty& dereference() noexcept
    {
        return *reinterpret_cast<ty*>(value);
    }

    template<class ty>
    ty absolute(ptrdiff_t rel_offset = 0x1, ptrdiff_t abs_offset = 0x0) noexcept
    {
        const auto jmp = value + rel_offset;
        const auto target = *reinterpret_cast<int32_t*>(jmp);
        if (target)
            // Base address + offset + size of next instruction + target address.
            return reinterpret_cast<ty>(jmp + abs_offset + 4 + target);
        return ty();
    }

private:
    uintptr_t value{ };
};
