#pragma once

#include "../render.h"
#include "../../core/input.h"

namespace menu {

    enum class tab_id {
        aimbot,
        visuals,
        misc,
        skins,
        config,

        count
    };

    enum class ctrl_type {
        checkbox,
        button,
        slider,
        dropdown,

        count
    };

    /* State */
    inline bool is_open{ };
    inline tab_id cur_tab{ };
    inline d2 base_pos{ 90, 90 };
    inline d2 size{ 600, 500 };

    /* Colors */
    constexpr clr4 inactive_color{ 64, 64, 64, 255 };
    constexpr clr4 hover_color{ 200, 100, 0, 255 };
    inline clr4 accent_color{ 255, 125, 0, 255 };

    void run() noexcept;
    void draw_watermark() noexcept;
    void handle_movement() noexcept;

    inline void toggle() noexcept
    {
        is_open = !is_open;
    }

}
