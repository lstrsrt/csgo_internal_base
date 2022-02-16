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

struct beam;

enum class beam_flag {
    start_entity = (1 << 0),
    end_entity = (1 << 1),
    fade_in = (1 << 2),
    fade_out = (1 << 3),
    sine_noise = (1 << 4),
    solid = (1 << 5),
    shade_in = (1 << 6),
    shade_out = (1 << 7),
    only_noise_once = (1 << 8),
    no_tile = (1 << 9),
    use_hitboxes = (1 << 10),
    start_visible = (1 << 11),
    end_visible = (1 << 12),
    is_active = (1 << 13),
    forever = (1 << 14),
    halo_beam = (1 << 15),
    reversed = (1 << 16)
};

enum class beam_type {
    points,
    sprite,
    disk,
    cylinder,
    follow,
    ring,
    spline,
    ring_point,
    laser,
    tesla
};

struct beam_info {
    beam_type type{ beam_type::points };
    base_entity* start_entity{ };
    int start_attachment{ };
    base_entity* end_entity{ };
    int end_attachment{ };
    vec3 start{ };
    vec3 end{ };
    int model_index{ -1 };
    const char* model_name{ };
    int halo_index{ -1 };
    const char* halo_name{ };
    float halo_scale{ };
    float life{ };
    float width{ 1.0 };
    float end_width{ 1.0 };
    float fade_length{ 100.0f };
    float amplitude{ };
    float brightness{ 255 };
    float speed{ };
    int start_frame{ };
    float frame_rate{ };
    float red{ };
    float green{ };
    float blue{ };
    bool renderable{ true };
    int segments{ -1 };
    bitfield<beam_flag> flags{ };
    vec3 center{ };
    float start_radius{ };
    float end_radius{ };

    explicit beam_info(const vec3& start, const vec3& end, const clr4& clr, float amplitude, float life, bitfield<beam_flag> flags = { }, 
        const char* model_name = "sprites/physbeam.vmt") noexcept
        : start(start), end(end), amplitude(amplitude), life(life), flags(flags), model_name(model_name)
    {
        red = clr.r;
        green = clr.g;
        blue = clr.b;
        brightness = clr.a;
    }
};

struct dlight {
    enum flag {
        no_world_illumination,
        no_model_illumination,
        add_displacement_alpha,
        sub_displacement_alpha
    };

    bitfield<flag> flags{ };
    vec3 origin{ };
    float radius{ };
    clr3 color{ };
    float die{ };
    float decay{ };
    float min_light{ };
    int key{ };
    int style{ };
    vec3 direction{ };
    float inner_angle{ };
    float outer_angle{ };
};

}
