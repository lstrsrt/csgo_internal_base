#pragma once

#include <array>

namespace cs {

struct key_values;
struct material;
using mat_handle = unsigned short;

struct material_system_config {
    PAD(0x32)
    unsigned int flags{ };
    PAD(0x11)
    bool fullbright{ };
};

using texture_group_t = const char*;

namespace texture_group {

    constexpr texture_group_t world = "World textures";
    constexpr texture_group_t model = "Model textures";
    constexpr texture_group_t static_prop = "StaticProp textures";
    constexpr texture_group_t particle = "Particle textures";
    constexpr texture_group_t sky_box = "SkyBox textures";
    constexpr texture_group_t client_effect = "ClientEffect textures";
    constexpr texture_group_t other = "Other textures";

}

}

namespace se {

struct material_system {
    VIRTUAL_FUNCTION(create_material, cs::material*, 83, (const char* name, cs::key_values* key_values), (this, name, key_values))
    VIRTUAL_FUNCTION(find_material, cs::material*, 84, (const char* name, cs::texture_group_t texture_group = cs::texture_group::model,
        bool complain = true, const char* complain_prefix = nullptr), (this, name, texture_group, complain, complain_prefix))
    VIRTUAL_FUNCTION(first_material, cs::mat_handle, 86, (), (this))
    VIRTUAL_FUNCTION(next_material, cs::mat_handle, 87, (cs::mat_handle handle), (this, handle))
    VIRTUAL_FUNCTION(get_material, cs::material*, 89, (cs::mat_handle handle), (this, handle))
    VIRTUAL_FUNCTION(get_material_count, int, 90, (), (this))
};

}
