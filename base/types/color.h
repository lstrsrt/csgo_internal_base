#pragma once

struct clr3;

struct clr4 {
    uint8_t r{ }, g{ }, b{ }, a{ };
};

struct clr3 {
    uint8_t r{ }, g{ }, b{ };

    auto to_clr4(uint8_t alpha = 255) const noexcept
    {
        return clr4(r, g, b, alpha);
    }
};
