#pragma once

#include "../render.h"
#include "../../core/input.h"
#include "controls.h"

namespace menu {

    inline bool is_active{ };
    inline clr3 accent_color{ 255, 125, 0 };
    constexpr clr3 inactive_ctrl_color{ 64, 64, 64 };
    inline d2 position{ 90, 90 };
    inline tab_id cur_tab{ tab_id::aimbot };
    
    void run() noexcept;
    
    inline void toggle() noexcept
    {
        is_active = !is_active;
    }

}
