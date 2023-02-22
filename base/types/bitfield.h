#pragma once

#include <compare>

template<class ty>
concept enumerator = __is_enum(ty);

namespace bitfield_ops {

    template<enumerator en>
    constexpr bool operator!(en bit) noexcept
    {
        return bit == static_cast<en>(0);
    }

    template<enumerator en>
    constexpr en operator&(en lhs, en rhs) noexcept
    {
        return static_cast<en>(std::to_underlying(lhs) & std::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator|(en lhs, en rhs) noexcept
    {
        return static_cast<en>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator^(en lhs, en rhs) noexcept
    {
        return static_cast<en>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator~(en bit) noexcept
    {
        return static_cast<en>(~std::to_underlying(bit));
    }

    template<enumerator en>
    en& operator|=(en& lhs, en rhs) noexcept
    {
        return lhs = static_cast<en>(std::to_underlying(lhs) | std::to_underlying(rhs));
    }

    template<enumerator en>
    en& operator&=(en& lhs, en rhs) noexcept
    {
        return lhs = static_cast<en>(std::to_underlying(lhs) &= std::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr bool has_bit(en lhs, en rhs) noexcept
    {
        return operator&(lhs, rhs) != static_cast<en>(0);
    }

}

using namespace bitfield_ops;

template<enumerator en>
class bitfield {
public:
    constexpr bitfield() noexcept = default;
    constexpr bitfield(en bits) noexcept
#ifndef __clang__
        : bits(bits) { }
#else
    {
        this->bits.value = bits;
    }
#endif
    constexpr bitfield(std::underlying_type_t<en> bits) noexcept
#ifndef __clang__
        : bits(static_cast<en>(bits)) { }
#else
    {
        this->bits.raw = bits;
    }
#endif

    constexpr operator bool() { return bits.raw; }
    constexpr operator en() noexcept { return value(); }
    constexpr auto operator=(int32_t rhs) noexcept { bits.value = static_cast<en>(rhs); }

    constexpr auto value() const noexcept { return bits.value; }
    constexpr auto& value() noexcept { return bits.value; }
    constexpr auto raw() const noexcept { return bits.raw; }
    constexpr auto& raw() noexcept { return bits.raw; }

    constexpr bool is_empty() const noexcept
    {
        return !bits.raw;
    }

    template<class... es> requires(std::same_as<es, en> && ...)
    constexpr bool is_set(es... t) const noexcept
    {
        return bits.raw & (std::to_underlying(t) | ...);
    }

    template<class... es> requires(std::same_as<es, en> && ...)
    constexpr void set(es... t) noexcept
    {
        bits.raw |= (std::to_underlying(t) | ...);
    }

    template<class... es> requires(std::same_as<es, en> && ...)
    constexpr void unset(es... t) noexcept
    {
        bits.raw &= (~std::to_underlying(t) | ...);
    }

    template<class... es> requires(std::same_as<es, en> && ...)
    constexpr void toggle(es... t) noexcept
    {
        bits.raw ^= (std::to_underlying(t) | ...);
    }

    constexpr void empty() noexcept
    {
        bits = en();
    }

private:
    union {
        en value;
        std::underlying_type_t<en> raw;
    } bits{ };
};
