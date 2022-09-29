#pragma once

namespace cs {

enum class model_type {
    bad,
    brush,
    sprite,
    studio
};

struct model {
    PAD(0x4)
    char name[MAX_PATH]{ };
    int	load_flags{ };
    int server_count{ };
    model_type type{ };
    int	flags{ };
    vec3 vec_mins{ };
    vec3 vec_maxs{ };
    float radius{ };
    key_values* key_values{ };
};

struct model_render_info {
    vec3 origin{ };
    vec3 angles{ };
    PAD(0x4)
    cs::renderable* renderable{ };
    const model* model{ };
    const mat3x4* model_to_world{ };
    const mat3x4* lighting_offset{ };
    const vec3* lighting_origin{ };
    int flags{ };
    int entity_index{ };
};

struct draw_model_results;

}
