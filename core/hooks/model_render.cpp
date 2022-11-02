#include "../hooks.h"
#include "../features/features.h"

static auto init_material() noexcept
{
    // return interfaces::material_system->find_material("debug/debugambientcube", cs::texture_group[cs::tg_model]);

    const auto kv = new cs::key_values[36];
    kv->init("VertexLitGeneric");
    kv->load_from_buffer("default.vmt", R"#("VertexLitGeneric" {
        "$basetexture"  "vgui/white_additive"
        "$ignorez"      "0"
        "$model"		"1"
        "$flat"			"0"
        "$nocull"		"1"
        "$halflambert"	"1"
        "$nofog"		"1"
        "$wireframe"	"0"
    })#");

    return interfaces::material_system->create_material("default.vmt", kv);
}

static void override_material(const clr4& clr, bool xqz) noexcept
{
    static auto material = init_material();
    material->set_flag(cs::material_flag::ignore_z, xqz);
    material->modulate_color(clr.r_base(), clr.g_base(), clr.b_base());
    material->modulate_alpha(clr.a_base());
    interfaces::model_render->forced_material_override(material);
}

void __fastcall hooks::draw_model_execute::fn(se::model_render* ecx, int, cs::mat_render_context* render_ctx,
                                              const cs::draw_model_state& state, const cs::model_render_info& info, mat3x4* bone_to_world)
{
    if (!local || ecx->is_forced_material_override())
        return original(ecx, render_ctx, state, info, bone_to_world);

    if (const auto entity = info.renderable->get_client_unknown()->get_base_entity()) {
        if (entity->is_player() && static_cast<cs::player*>(entity)->is_enemy()) {
            override_material({ 75, 75, 200, 255 }, true);
            original(ecx, render_ctx, state, info, bone_to_world);
            ecx->forced_material_override(nullptr);
            override_material({ 200, 60, 60, 255 }, false);
        }
    }

    original(ecx, render_ctx, state, info, bone_to_world);
    interfaces::model_render->forced_material_override(nullptr);
}
