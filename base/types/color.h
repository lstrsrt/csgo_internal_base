#pragma once

struct clr3;

struct clr4 {
    uint8_t r{ }, g{ }, b{ }, a{ };

    constexpr clr4() noexcept = default;
    constexpr clr4(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept
        : r(r), g(g), b(b), a(a) { }

    float r_base() const noexcept { return r / 255.f; }
    float g_base() const noexcept { return g / 255.f; }
    float b_base() const noexcept { return b / 255.f; }
    float a_base() const noexcept { return a / 255.f; }
};

struct clr3 {
    uint8_t r{ }, g{ }, b{ };

    constexpr clr3() noexcept = default;
    constexpr clr3(uint8_t r, uint8_t g, uint8_t b) noexcept
        : r(r), g(g), b(b) { }

    auto to_clr4(uint8_t alpha = 255) const noexcept
    {
        return clr4(r, g, b, alpha);
    }
};
