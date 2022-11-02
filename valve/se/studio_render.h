#pragma once

#include "../cs/material.h"

namespace cs {

    enum class override_type {
        normal,
        build_shadows,
        depth_write,
        selective,
        ssao_depth_write
    };

}

namespace se {

struct studio_render {
    VIRTUAL_FUNCTION(set_color_modulation, void, 27, (const clr3& clr), (this, clr))
    VIRTUAL_FUNCTION(set_alpha_modulation, void, 28, (float alpha), (this, alpha))
    VIRTUAL_FUNCTION(forced_material_override, void, 33, (cs::material* material,
        cs::override_type type = cs::override_type::normal, int overrides = 0), (this, material, type, overrides))

    PAD(0x240)
    float color_modulation[3]{ };
    float alpha_modulation{ };
    cs::material* forced_material[4]{ };
    cs::override_type forced_material_type{ };
    int forced_material_index[4]{ };
    int forced_material_index_count{ };

    inline bool is_forced_material_override() const noexcept
    {
        if (!forced_material[0])
            return forced_material_type == cs::override_type::depth_write || forced_material_type == cs::override_type::ssao_depth_write;

        return *reinterpret_cast<const uint32_t*>(forced_material[0]->get_name() + 4) == 'wolg';
    }

    inline void set_color(const clr4& clr) noexcept
    {
        color_modulation[0] = clr.r_base();
        color_modulation[1] = clr.g_base();
        color_modulation[2] = clr.b_base();
        alpha_modulation = clr.a_base();
    }

    inline void set_material(cs::material* mat) noexcept
    {
        forced_material_type = cs::override_type::normal;
        forced_material[0] = mat;
        forced_material_index[0] = -1;
        forced_material_index_count = 0;
    }

};

}
