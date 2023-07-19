#pragma once

#include "../../base/base.h"
#include "../../valve/cs/cs.h"
#include "../../memory/interfaces.h"
#include "../cheat.h"
#include "variables.h"

inline namespace features {

using cheat::local;

namespace visuals::chams {

    inline std::vector<cs::material*> materials{ };

    bool initialize() noexcept;
    void on_new_map() noexcept;
    void override_material(const clr4& clr, bool occluded) noexcept;

}

namespace visuals::esp {

    struct {
        int left{ };
        int top{ };
        int right{ };
        int bottom{ };
        int width{ };
        int height{ };
        uint8_t alpha{ };
    } inline box;

    void run() noexcept;
    void draw_box() noexcept;
    void draw_name(std::string_view name) noexcept;
    void draw_weapon(cs::weapon* weapon) noexcept;
    void draw_health(int health) noexcept;
    bool generate_box(cs::base_entity* entity) noexcept;

}

}

