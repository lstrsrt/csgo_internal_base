#pragma once

#include <type_traits>

template<class ty>
inline constexpr auto to_underlying(ty v)
{
    return static_cast<std::underlying_type_t<ty>>(v);
}

namespace bitfield_operators {

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    constexpr ty operator&(const ty& lhs, const ty& rhs) noexcept
    {
        return static_cast<ty>(to_underlying(lhs) & to_underlying(rhs));
    }

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    constexpr ty operator^(const ty& lhs, const ty& rhs) noexcept
    {
        return static_cast<ty>(to_underlying(lhs) ^ to_underlying(rhs));
    }

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    constexpr ty operator|(const ty& lhs, const ty& rhs) noexcept
    {
        return static_cast<ty>(to_underlying(lhs) | to_underlying(rhs));
    }

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    constexpr ty operator~(const ty& bit) noexcept
    {
        return static_cast<ty>(~to_underlying(bit));
    }

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    inline ty& operator&=(ty& lhs, const ty& rhs) noexcept
    {
        return lhs = static_cast<ty>(to_underlying(lhs) &= to_underlying(rhs));
    }

    template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
    inline ty& operator|=(ty& lhs, const ty& rhs) noexcept
    {
        return lhs = static_cast<ty>(to_underlying(lhs) | to_underlying(rhs));
    }

}

using namespace bitfield_operators;

template<class ty>
class bitfield
{
public:
    bitfield() noexcept = default;

    bitfield(ty bits) noexcept
        : m_bits(bits)
    {
    }

    constexpr operator ty() noexcept
    {
        return raw();
    }

    auto operator=(int rhs) noexcept
    {
        m_bits.m_value = static_cast<ty>(rhs);
    }

    auto value() noexcept
    {
        return m_bits.m_value;
    }

    auto raw() noexcept
    {
        return m_bits.m_raw;
    }

    constexpr void set(const ty bit) noexcept
    {
        m_bits.m_raw |= to_underlying(bit);
    }

    constexpr void unset(const ty bit) noexcept
    {
        m_bits.m_raw &= ~to_underlying(bit);
    }

    constexpr bool is_set(const ty bit) const noexcept
    {
        return m_bits.m_raw & to_underlying(bit);
    }

    constexpr void empty() noexcept
    {
        m_bits = ty();
    }

    constexpr bool is_empty() noexcept
    {
        return !m_bits.m_raw;
    }

private:

    union {
        ty m_value;
        std::underlying_type_t<ty> m_raw;
    } m_bits{};

};
