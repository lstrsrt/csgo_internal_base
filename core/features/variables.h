#pragma once

#include "../config.h"

#define ADD_VARIABLE(name, type, preset) inline auto name = cfg::add_item({ preset, fnv1a::hash(#name), cfg::item_type::type })

namespace vars {

    ADD_VARIABLE(prediction, boolean, false);

    ADD_VARIABLE(radar_reveal, boolean, false);
    ADD_VARIABLE(esp, boolean, false);

    ADD_VARIABLE(fov, f32, 90.0f);
    ADD_VARIABLE(viewmodel_fov, f32, 68.0f);
    ADD_VARIABLE(disable_postprocessing, boolean, true);

    ADD_VARIABLE(infinite_crouch, boolean, false);

}
