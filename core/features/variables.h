#pragma once

#include "../config.h"

#define ADD_VARIABLE(name, type, preset) inline auto name = cfg::add_item({ preset, fnv1a::hash(#name), cfg::item_type::type })
#define ADD_INT(name, preset) ADD_VARIABLE(name, i32, preset)
#define ADD_FLOAT(name, preset) ADD_VARIABLE(name, f32, preset)
#define ADD_BOOL(name, preset) ADD_VARIABLE(name, boolean, preset)
#define ADD_COLOR3(name, preset) ADD_VARIABLE(name, clr3, preset)
#define ADD_COLOR4(name, preset) ADD_VARIABLE(name, clr4, preset)
#define ADD_BOOL_VEC(name, preset) ADD_VARIABLE(name, bool_vec, preset)
#define ADD_KEYBIND(name, preset) ADD_VARIABLE(name, keybind, preset)

namespace vars {

    ADD_BOOL(prediction, false);

    ADD_BOOL(radar_reveal, false);
    ADD_BOOL(esp, false);
    ADD_FLOAT(fov, 90.0f);
    ADD_FLOAT(viewmodel_fov, 68.0f);
    ADD_BOOL(disable_postprocessing, true);

    ADD_BOOL(infinite_crouch, false);
    ADD_KEYBIND(panic_key, (keybind{ VK_DELETE, input::key_type::toggle }));

    ADD_INT(test_multi, 0);
    ADD_COLOR3(test_clr3, (clr3{ 255, 255, 255 }));
    ADD_COLOR4(test_clr4, (clr4{ 255, 0, 255, 0 }));
    ADD_BOOL_VEC(test_vec, (std::vector<bool>{ false, true, false, true }));

}
