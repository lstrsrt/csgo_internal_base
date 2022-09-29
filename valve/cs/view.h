#pragma once

namespace cs {

struct base_entity;

enum class frame_stage {
    start,
    net_update_start,
    net_update_post_data_update_start,
    net_update_post_data_update_end,
    net_update_end,
    render_start,
    render_end
};

enum class observer_mode {
    none,
    deathcam,
    freezecam,
    fixed,
    in_eye,
    chase,
    roaming,
    other
};

struct view_setup {
    int x{ };
    int old_x{ };
    int y{ };
    int old_y{ };
    int width{ };
    int old_width{ };
    int height{ };
    int old_height{ };
    bool ortho{ };
    float ortho_left{ };
    float ortho_top{ };
    float ortho_right{ };
    float ortho_bottom{ };
    bool custom_view_matrix{ };
    mat3x4 custom_matrix{ };
    PAD(0x48)
    float fov{ };
    float viewmodel_fov{ };
    vec3 origin{ };
    angle angles{ };
};

}
