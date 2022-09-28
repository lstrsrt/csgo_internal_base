#pragma once

namespace se {

struct studio_render_context {
    VIRTUAL_FUNCTION(set_color_modulation, void, 27, (const clr3& clr), (this, clr))
    VIRTUAL_FUNCTION(set_alpha_modulation, void, 28, (float alpha), (this, alpha))
    VIRTUAL_FUNCTION(forced_material_override, void, 33, (cs::material* material,
        int override_type = 0, int overrides = 0), (this, material, override_type, overrides))
};

}
