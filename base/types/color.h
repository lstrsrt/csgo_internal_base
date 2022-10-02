#pragma once

struct clr3;

struct clr4 {
    constexpr clr4() noexcept = default;
    constexpr clr4(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
        : r(r), g(g), b(b), a(a) { }

    uint8_t r{ }, g{ }, b{ }, a{ };
};

struct clr3 {
    constexpr clr3() noexcept = default;
    constexpr clr3(uint8_t r, uint8_t g, uint8_t b) noexcept
        : r(r), g(g), b(b) { }

    uint8_t r{ }, g{ }, b{ };

    auto to_clr4(uint8_t alpha = 255) const noexcept
    {
        return clr4(r, g, b, alpha);
    }
};
