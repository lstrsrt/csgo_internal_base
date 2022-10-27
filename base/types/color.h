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

    static clr4 white(uint8_t a = 255) noexcept { return clr4(255, 255, 255, a); }
    static clr4 black(uint8_t a = 255) noexcept { return clr4(0, 0, 0, a); }
    static clr4 red(uint8_t a = 255) noexcept { return clr4(255, 0, 0, a); }
    static clr4 green(uint8_t a = 255) noexcept { return clr4(0, 255, 0, a); }
    static clr4 blue(uint8_t a = 255) noexcept { return clr4(0, 0, 255, a); }
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
