#pragma once

#include <compare>

namespace bitfield_ops {

    template<enumerator en>
    constexpr bool operator!(en bit) noexcept
    {
        return bit == static_cast<en>(0);
    }

    template<enumerator en>
    constexpr en operator&(en lhs, en rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) & util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator|(en lhs, en rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) | util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator^(en lhs, en rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) ^ util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator<<(en lhs, en rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) << util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator>>(en lhs, en rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) >> util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator~(en bit) noexcept
    {
        return static_cast<en>(~util::to_underlying(bit));
    }

    template<enumerator en>
    inline en& operator|=(en& lhs, en rhs) noexcept
    {
        return lhs = static_cast<en>(util::to_underlying(lhs) | util::to_underlying(rhs));
    }

    template<enumerator en>
    inline en& operator&=(en& lhs, en rhs) noexcept
    {
        return lhs = static_cast<en>(util::to_underlying(lhs) &= util::to_underlying(rhs));
    }

    template<enumerator en>
    inline en& operator<<=(en& lhs, std::integral auto rhs) noexcept
    {
        return lhs = static_cast<en>(util::to_underlying(lhs) <<= rhs);
    }

    template<enumerator en>
    inline en& operator>>=(en& lhs, std::integral auto rhs) noexcept
    {
        return lhs = static_cast<en>(util::to_underlying(lhs) >>= rhs);
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
    constexpr bitfield(std::underlying_type<en> bits) noexcept
#ifndef __clang__
        : bits(bits) { }
#else
    {
        this->bits.raw = bits;
    }
#endif

    constexpr operator bool() { return bits.raw; }
    constexpr operator en() noexcept { return value(); }

    constexpr auto operator=(int32_t rhs) noexcept
    {
        bits.value = static_cast<en>(rhs);
    }

    constexpr auto operator<=>(en rhs) const noexcept
    {
        return bits.raw <=> util::to_underlying(rhs);
    }

    constexpr auto value() noexcept
    {
        return bits.value;
    }

    constexpr auto raw() noexcept
    {
        return bits.raw;
    }

    constexpr void set(en bit) noexcept
    {
        bits.raw |= util::to_underlying(bit);
    }

    constexpr void unset(en bit) noexcept
    {
        bits.raw &= ~util::to_underlying(bit);
    }

    constexpr void toggle(en bit) noexcept
    {
        bits.raw ^= util::to_underlying(bit);
    }

    constexpr void shift_l(std::integral auto i) noexcept
    {
        bits.raw <<= i;
    }

    constexpr void shift_r(std::integral auto i) noexcept
    {
        bits.raw >>= i;
    }

    constexpr void empty() noexcept
    {
        bits = en();
    }

    constexpr bool is_set(en bit) const noexcept
    {
        return bits.raw & util::to_underlying(bit);
    }

    constexpr bool is_empty() const noexcept
    {
        return !bits.raw;
    }

private:
    union {
        en value;
        std::underlying_type_t<en> raw;
    } bits{ };
};
