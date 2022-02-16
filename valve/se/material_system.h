#pragma once

#include <unordered_map>

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

enum class e_texture_group {
    tg_world,
    tg_model,
    tg_static_prop,
    tg_particle,
    tg_skybox,
    tg_client_effects,
    tg_other
};

using enum e_texture_group;

inline std::unordered_map<e_texture_group, const char*> texture_group{
    { tg_world, "World textures" },
    { tg_model, "Model textures" },
    { tg_static_prop, "StaticProp textures" },
    { tg_particle, "Particle textures" },
    { tg_skybox, "SkyBox textures" },
    { tg_client_effects, "ClientEffect textures" },
    { tg_other, "Other textures" }
};

}

namespace se {

struct material_system {
    VIRTUAL_FUNCTION(create_material, cs::material*, 83, (const char* name, cs::key_values* key_values), (this, name, key_values))
    VIRTUAL_FUNCTION(find_material, cs::material*, 84, (const char* name, const char* texture_group_name = cs::texture_group[cs::tg_model],
        bool complain = true, const char* complain_prefix = nullptr), (this, name, texture_group_name, complain, complain_prefix))
    VIRTUAL_FUNCTION(first_material, cs::mat_handle, 86, (), (this))
    VIRTUAL_FUNCTION(next_material, cs::mat_handle, 87, (cs::mat_handle handle), (this, handle))
    VIRTUAL_FUNCTION(get_material, cs::material*, 89, (cs::mat_handle handle), (this, handle))
    VIRTUAL_FUNCTION(get_material_count, int, 90, (), (this))
};

}
