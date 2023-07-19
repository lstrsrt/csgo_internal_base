#include "features.h"

static auto create_material(std::string_view path, cs::texture_group_t group = cs::texture_group::model) noexcept
{
    return interfaces::material_system->find_material(path.data(), group);
}

static auto create_material(std::string_view name, std::string_view shader, std::string_view params) noexcept
{
    const auto kv = new cs::key_values[36];
    kv->init(shader.data());
    kv->load_from_buffer(name.data(), params.data());
    return interfaces::material_system->create_material(name.data(), kv);
}

bool visuals::chams::initialize() noexcept
{
    materials.clear();
    materials.emplace_back(create_material("default.vmt",
                                           "VertexLitGeneric",
                                           R"#("VertexLitGeneric" {
                                               "$basetexture"  "vgui/white_additive"
                                               "$ignorez"       "0"
                                               "$model"	        "1"
                                               "$flat"		    "0"
                                               "$nocull"	    "1"
                                               "$halflambert"	"1"
                                               "$nofog"	        "1"
                                               "$wireframe"	    "0"
                                           })#"));
    return true;
}

void visuals::chams::on_new_map() noexcept
{
    for (auto* mat : materials) {
        VERIFY(mat);
        mat->increment_ref_count();
    }
}

void visuals::chams::override_material(const clr4& clr, bool occluded) noexcept
{
    auto* material = materials[0]; // This should select from a config in practice
    material->set_flag(cs::material_flag::ignore_z, occluded);
    material->modulate(clr);
    interfaces::studio_render->forced_material_override(material);
}
