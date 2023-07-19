#include "../hooks.h"
#include "../features/features.h"

void __fastcall hooks::draw_model::fn(se::studio_render* ecx, int, cs::draw_model_results* results, const cs::draw_model_info& info,
    mat3x4* bone_to_world, float* flex_weights, float* flex_delayed_weights, const vec3& origin, int flags)
{
    if (!local || !local.in_game)
        return original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);

    if (!info.client_entity || ecx->is_forced_material_override())
        return original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);

    original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);

    [[maybe_unused]] static bool once = [] { return visuals::chams::initialize(); }();

    bool clear{ };
    const auto draw = [&](const clr4& clr, bool occluded)
    {
        clear = true;
        visuals::chams::override_material(clr, occluded);
        original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);
    };

    auto unknown = info.client_entity->get_client_unknown();
    const auto class_id = unknown->get_networkable()->get_client_class()->id;
    if (class_id == cs::class_id::cs_player &&
        static_cast<cs::player*>(unknown->get_base_entity())->is_enemy()) {
        draw({ 75, 75, 200, 255 }, true);
        draw({ 200, 60, 60, 255 }, false);
    }

    original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);
    if (clear)
        ecx->forced_material_override(nullptr);

    // bool clear{ };
    // const auto draw_chams = [&](const clr4& clr, bool occluded)
    // {
    //     static auto material = create_material();
    //     material->set_flag(cs::material_flag::ignore_z, occluded);
    //     material->modulate(clr);
    //     ecx->forced_material_override(material);
    //     original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);
    //     clear = true;
    // };
    //
    // auto unknown = info.client_entity->get_client_unknown();
    // const auto class_id = unknown->get_networkable()->get_client_class()->id;
    // if (class_id == cs::class_id::cs_player &&
    //     static_cast<cs::player*>(unknown->get_base_entity())->is_enemy()) {
    //     draw_chams({ 75, 75, 200, 255 }, true);
    //     draw_chams({ 200, 60, 60, 255 }, false);
    // }
    //
    // original(ecx, results, info, bone_to_world, flex_weights, flex_delayed_weights, origin, flags);
    // if (clear)
    //     interfaces::studio_render->forced_material_override(nullptr);
}
