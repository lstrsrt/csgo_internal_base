#pragma once

namespace bitfield_ops {

    template<enumerator en>
    constexpr en operator&(const en& lhs, const en& rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) & util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator|(const en& lhs, const en& rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) | util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator^(const en& lhs, const en& rhs) noexcept
    {
        return static_cast<en>(util::to_underlying(lhs) ^ util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator<<(const en& lhs, const en& rhs) noexcept
    {
        return static_cast< en >(util::to_underlying(lhs) << util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator>>(const en& lhs, const en& rhs) noexcept
    {
        return static_cast< en >(util::to_underlying(lhs) >> util::to_underlying(rhs));
    }

    template<enumerator en>
    constexpr en operator~(const en& bit) noexcept
    {
        return static_cast<en>(~util::to_underlying(bit));
    }

    template<enumerator en>
    inline en& operator|=(en& lhs, const en& rhs) noexcept
    {
        return lhs = static_cast< en >(util::to_underlying(lhs) | util::to_underlying(rhs));
    }

    template<enumerator en>
    inline en& operator&=(en& lhs, const en& rhs) noexcept
    {
        return lhs = static_cast<en>(util::to_underlying(lhs) &= util::to_underlying(rhs));
    }

    template<enumerator en>
    inline en& operator<<= (en & lhs, const en & rhs) noexcept
    {
        return lhs = static_cast< en >(util::to_underlying(lhs) <<= util::to_underlying(rhs));
    }

    template<enumerator en>
    inline en& operator>>=(en& lhs, const en& rhs) noexcept
    {
        return lhs = static_cast< en >(util::to_underlying(lhs) >>= util::to_underlying(rhs));
    }

}

using namespace bitfield_ops;

template<enumerator en>
class bitfield {
public:
    constexpr bitfield() noexcept = default;

    constexpr bitfield(en bits) noexcept
        : m_bits(bits) { }

    constexpr bitfield(std::underlying_type<en> bits) noexcept
        : m_bits(bits) { }

    constexpr operator en() noexcept
    {
        return raw();
    }

    constexpr auto operator=(int32_t rhs) noexcept
    {
        m_bits.m_value = static_cast<en>(rhs);
    }

    constexpr auto value() noexcept
    {
        return m_bits.m_value;
    }

    constexpr auto raw() noexcept
    {
        return m_bits.m_raw;
    }

    constexpr void set(const en bit) noexcept
    {
        m_bits.m_raw |= util::to_underlying(bit);
    }

    constexpr void unset(const en bit) noexcept
    {
        m_bits.m_raw &= ~util::to_underlying(bit);
    }

    constexpr bool toggle(const en bit) noexcept
    {
        return m_bits.m_raw ^= util::to_underlying(bit);
    }

    constexpr bool shift_l(const en bit) noexcept
    {
        return m_bits.m_raw <<= util::to_underlying(bit);
    }

    constexpr bool shift_r(const en bit) noexcept
    {
        return m_bits.m_raw >>= util::to_underlying(bit);
    }

    constexpr void empty() noexcept
    {
        m_bits = en();
    }

    constexpr bool is_set(const en bit) const noexcept
    {
        return m_bits.m_raw & util::to_underlying(bit);
    }

    constexpr bool is_empty() const noexcept
    {
        return !m_bits.m_raw;
    }

private:
    union {
        en m_value;
        std::underlying_type_t<en> m_raw;
    } m_bits{};
};
