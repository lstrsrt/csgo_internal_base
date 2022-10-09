#pragma once

template<class ty>
concept floating_point = std::is_floating_point_v<ty>;

template<class ty>
concept enumerator = __is_enum(ty);

template<class ty>
concept string_like = requires(ty t)
{
    t.data();
    t.substr();
};

namespace util {

    template<enumerator en>
    inline constexpr auto to_underlying(en e)
    {
        return static_cast<std::underlying_type_t<en>>(e);
    }

    template<class ty>
    inline constexpr ty force_cast(const auto a)
    {
        union { decltype(a) x; ty y; } u{ a };
        return u.y;
    }

    template<class ty> requires std::is_array_v<ty>
    inline constexpr auto array_size(const ty& x)
    {
        return sizeof(x) / sizeof(x[0]);
    }

}
