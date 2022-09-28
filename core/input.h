#pragma once

#include <array>
#include <Windows.h>

#include "../base/math.h"

namespace input {

    enum class key_state {
        up,
        down,
        toggled
    };

    enum class key_type {
        off,
        always,
        hold,
        toggle,
        release
    };

    struct keybind {
        uint32_t code;
        key_type type;
    };

    inline std::array<key_state, 255u> key_states{ };
    inline d2 mouse_pos{ };

    bool is_key_active(keybind key) noexcept;
    bool is_hovering_item(const d2& item_pos, const d2& item_size) noexcept;
    void process(UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

}

using input::keybind;
