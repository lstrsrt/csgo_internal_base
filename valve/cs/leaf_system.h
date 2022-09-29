#pragma once

namespace cs {

enum class render_flag : uint32_t {
    bounds_always_recompute = (1 << 6),
    force_opaque_pass = (1 << 8)
};

struct renderable_info {
    cs::renderable* renderable{ };
    PAD(0x4)
    int enum_count{ };
    int render_frame{ };
    unsigned short first_shadow{ };
    unsigned short leaf_list{ };
    short area{ };
    bitfield<render_flag> flags{ };
    vec3 bloated_abs_mins{ };
    vec3 bloated_abs_maxs{ };
    vec3 abs_mins{ };
    vec3 abs_maxs{ };
};

struct renderable_entry {
    cs::renderable* renderable{ };
    unsigned short world_list_into_leaf{ };
    uint8_t alpha{ };
    uint8_t flags{ };
    bool is_combined_model{ };
};

enum render_group {
    opaque,
    translucent,
    translucent_ignore_z,
    count
};

struct world_list_info {
    int view_fog_volume{ };
    int leaf_count{ };
    bool has_water{ };
    uint64_t* leaf_data_list{ };
};

struct renderables_list {
    float max_dist_sqr{ };
    float min_dist_sqr{ };
    float falloff_factor{ };
    renderable_entry render_groups[render_group::count][4096]{ };
    int render_group_counts[render_group::count]{ };
    int bone_setup_dependency_count{ };
    cs::renderable* bone_setup_dependency[64];
};

struct setup_render_info {
    mutable world_list_info* world_list_info{ };
    mutable renderables_list* render_list{ };
};

}
