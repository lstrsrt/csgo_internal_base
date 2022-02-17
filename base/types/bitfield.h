#pragma once

#include <type_traits>

namespace bitfield_operators {

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
constexpr ty operator&(const ty& lhs, const ty& rhs) noexcept
{
    return static_cast<ty>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
constexpr ty operator^(const ty& lhs, const ty& rhs) noexcept
{
    return static_cast<ty>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
constexpr ty operator|(const ty& lhs, const ty& rhs) noexcept
{
    return static_cast<ty>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
constexpr ty operator~(const ty& bit) noexcept
{
    return static_cast<ty>(~std::to_underlying(bit));
}

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
inline ty& operator&=(ty& lhs, const ty& rhs) noexcept
{
    return lhs = static_cast<ty>(std::to_underlying(lhs) &= std::to_underlying(rhs));
}

template<class ty, std::enable_if_t<std::is_enum_v<ty>, int> = 0>
inline ty& operator|=(ty& lhs, const ty& rhs) noexcept
{
    return lhs = static_cast<ty>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

}

using namespace bitfield_operators;

template<class ty>
class bitfield {
private:
    ty bits{ };

public:
    bitfield() noexcept = default;
    bitfield(ty bits) noexcept
        : bits(bits) { }

    constexpr operator ty() const noexcept
    {
        return to_underlying();
    }

    constexpr auto to_int() const noexcept
    {
        return static_cast<int>(bits);
    }

    constexpr auto to_underlying() noexcept
    {
        return bits;
    }

    constexpr bool is_set(ty bit) const noexcept
    {
        return std::to_underlying(bits) & std::to_underlying(bit);
    }

    constexpr void set(const ty bit) noexcept
    {
        bits |= bit;
    }

    constexpr void unset(const ty bit) noexcept
    {
        bits = bits & ~bit;
    }

    constexpr void empty() noexcept
    {
        bits = ty();
    }

    constexpr bool is_empty() noexcept
    {
        return !std::to_underlying(bits);
    }
};
