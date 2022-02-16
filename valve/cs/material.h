#pragma once

namespace cs {

struct material_var {
    VIRTUAL_FUNCTION(set_value, void, 4, (float value), (this, value))
    VIRTUAL_FUNCTION(set_vector_value, void, 11, (float x, float y, float z), (this, x, y, z))
    VIRTUAL_FUNCTION(set_vector_component_value, void, 26, (float x, int component), (this, x, component))
};

enum class material_flag {
    debug = (1 << 0),
    no_debug_override = (1 << 1),
    no_draw = (1 << 2),
    use_in_fillrate_mode = (1 << 3),
    vertex_color = (1 << 4),
    vertex_alpha = (1 << 5),
    self_illum = (1 << 6),
    additive = (1 << 7),
    alphatest = (1 << 8),
    z_nearer = (1 << 10),
    model = (1 << 11),
    flat = (1 << 12),
    no_cull = (1 << 13),
    no_fog = (1 << 14),
    ignore_z = (1 << 15),
    decal = (1 << 16),
    base_alpha_env_map_mask = (1 << 20),
    translucent = (1 << 21),
    normal_map_alpha_env_map_mask = (1 << 22),
    opaque_texture = (1 << 24),
    suppress_decals = (1 << 26),
    half_lambert = (1 << 27),
    wireframe = (1 << 28)
};

struct material {
    VIRTUAL_FUNCTION(find_var, material_var*, 11, (const char* name, bool* found, bool complain = true), (this, name, found, complain))
    VIRTUAL_FUNCTION(modulate_alpha, void, 27, (float alpha), (this, alpha))
    VIRTUAL_FUNCTION(modulate_color, void, 28, (float r, float g, float b), (this, r, g, b))
    VIRTUAL_FUNCTION(set_flag, void, 29, (material_flag var_flag, bool state), (this, var_flag, state))
    VIRTUAL_FUNCTION(get_flag, bool, 30, (material_flag var_flag), (this, var_flag))
    VIRTUAL_FUNCTION(is_error_material, bool, 42, (), (this))
};

struct mat_render_context;

}
