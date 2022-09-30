#include "cheat.h"
#include "../render/menu/menu.h"
#include "features/features.h"
#include "hooks.h"
#include "features/cache.h"
#include "features/variables.h"

void hooks::initialize() noexcept
{
    D3DDEVICE_CREATION_PARAMETERS creation_params{ };
    interfaces::dx9_device->GetCreationParameters(&creation_params);
    if (creation_params.hFocusWindow) {
        game_window = creation_params.hFocusWindow;
        original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));
    }

    hook_func = memory::find_bytes(dll::game_overlay_renderer, PATTERN("55 8B EC 51 8B 45 10 C7")).cast<decltype(hook_func)>();
    unhook_func = memory::find_bytes(dll::game_overlay_renderer, PATTERN("E8 ? ? ? ? 83 C4 08 FF 15")).absolute<decltype(unhook_func)>();

    SET_VT_HOOK(interfaces::client, frame_stage_notify, 37);
    SET_VT_HOOK(interfaces::client, create_move_proxy, 22);
    SET_VT_HOOK(interfaces::client_mode, override_view, 18);
    SET_VT_HOOK(interfaces::client_mode, get_viewmodel_fov, 35);
    SET_VT_HOOK(interfaces::engine, is_connected, 27);
    SET_VT_HOOK(interfaces::vgui, paint, 14);
    SET_VT_HOOK(interfaces::material_system, override_config, 21);
    SET_VT_HOOK(interfaces::model_render, draw_model_execute, 21);
    SET_VT_HOOK(interfaces::bsp_query, list_leaves_in_box, 6);
    SET_VT_HOOK(interfaces::surface, lock_cursor, 67);

    SET_SIG_HOOK(dll::client, "55 8B EC 51 8B 45 0C 53 56 8B F1 57", on_add_entity);
    SET_SIG_HOOK(dll::client, "55 8B EC 51 8B 45 0C 53 8B D9 56 57 83 F8 FF 75 07", on_remove_entity);

    SET_PROXY("CBaseEntity->m_bSpotted", spotted);

    cache::initialize();
}

void hooks::end() noexcept
{
    for (auto* a : interfaces::hooked_tables)
        static_cast<interface_holder<void*>*>(a)->restore();

    unset(on_add_entity::fn);
    unset(on_remove_entity::fn);

    netvars::unset_proxy("CBaseEntity->m_bSpotted"_hash, spotted::original);

    SetWindowLongPtrA(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc));
}

LRESULT CALLBACK hooks::wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle }))
        menu::toggle();

    interfaces::input_system->enable_input(!menu::is_active);

    if (menu::is_active)
        return TRUE;

    return CallWindowProcA(original_wnd_proc, hwnd, msg, wparam, lparam);
}

static void __stdcall create_move(int sequence_nr, float input_sample_time, bool is_active, bool& send_packet)
{
    hooks::create_move_proxy::original(interfaces::client, sequence_nr, input_sample_time, is_active);

    if (!is_active || !send_packet)
        return;

    if (!local.update())
        return;

    auto cmd = &interfaces::input->cmds[sequence_nr % cs::multiplayer_backup];
    auto verified_cmd = &interfaces::input->verified_cmds[sequence_nr % cs::multiplayer_backup];
    if (!cmd || !verified_cmd)
        return;

    local.cur_cmd = cmd;

    if (config::get<bool>(vars::infinite_crouch))
        cmd->buttons.set(cs::cmd_button::bullrush);

    prediction::start(cmd);
    // aimbot::run(cmd);
    prediction::end();

    local.view = cmd->view_angles;

    verified_cmd->cmd = *cmd;
    verified_cmd->crc = cmd->get_checksum();
}

__declspec(naked) void __fastcall hooks::create_move_proxy::fn(se::client_dll* ecx, int, int sequence_nr,
    float input_sample_time, bool is_active)
{
    __asm {
        push ebp
        mov  ebp, esp

        push  ebx // send_packet
        push  esp
        push  dword ptr[ebp + 16]
        push  dword ptr[ebp + 12]
        push  dword ptr[ebp + 8]
        call  create_move
        pop   ebx

        pop   ebp
        ret   12
    }
}

void __fastcall hooks::frame_stage_notify::fn(se::client_dll* ecx, int, cs::frame_stage frame_stage)
{
    if (!local.in_game)
        return original(ecx, frame_stage);

    static auto override_postprocessing_disable = *memory::find_bytes(dll::client, PATTERN("83 EC 4C 80 3D")).offset(0x5).cast<bool**>();

    switch (frame_stage) {
    case cs::frame_stage::render_start:
        *override_postprocessing_disable = config::get<bool>(vars::disable_postprocessing);
        break;
    case cs::frame_stage::net_update_post_data_update_end:
        local.update();
        break;
    }

    return original(ecx, frame_stage);
}

void __fastcall hooks::override_view::fn(se::client_mode* ecx, int, cs::view_setup* view)
{
    if (!local.update())
        return original(ecx, view);

    if (local->is_alive()) {
        if (!local->is_scoping())
            view->fov = config::get<float>(vars::fov);
    }

    return original(ecx, view);
}

float __fastcall hooks::get_viewmodel_fov::fn(se::client_mode* ecx, int)
{
    if (!local || (local && local->is_scoping()))
        return original(ecx);

    return config::get<float>(vars::viewmodel_fov);
}

bool __fastcall hooks::is_connected::fn(se::engine_client* ecx, int)
{
    static const auto is_loadout_allowed = memory::find_bytes(dll::client, PATTERN("84 C0 75 05 B0 01 5F"));
    if (config::get<bool>(vars::unlock_inventory) && _ReturnAddress() == is_loadout_allowed)
        return false;

    return original(ecx);
}

void __fastcall hooks::paint::fn(se::vgui* ecx, int, cs::paint_mode mode)
{
    static bool once = []() { return render::initialize(); }();

    if (ecx->static_transition_panel && (mode & cs::paint_mode::paint_ui_panels)) {
        interfaces::surface->start_drawing();

        render::text(render::fonts::watermark, { 15, 15 }, L"hello");
        menu::run();

        interfaces::surface->finish_drawing();
    }

    return original(ecx, mode);
}

bool __fastcall hooks::override_config::fn(se::material_system* ecx, int, cs::material_system_config* config, bool force_update)
{
    config->fullbright = config::get<bool>(vars::fullbright);
    return original(ecx, config, force_update);
}

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

static void override_material(bool xqz) noexcept
{
    static auto material = init_material();
    material->set_flag(cs::material_flag::ignore_z, xqz);
    material->modulate_color(0.8f, 0.3f, 0.0f);
    material->modulate_alpha(1.0f);
    interfaces::model_render->forced_material_override(material);
}

void __fastcall hooks::draw_model_execute::fn(se::model_render* ecx, int, cs::mat_render_context* render_ctx, const cs::draw_model_state& state,
    const cs::model_render_info& info, mat3x4* bone_to_world)
{
    if (ecx->is_forced_material_override())
        return original(ecx, render_ctx, state, info, bone_to_world);

    const auto model_name = interfaces::model_info->get_model_name(info.model);
    if (strstr(model_name, "models/player")) {
        const auto player = interfaces::entity_list->get<cs::player*>(info.entity_index);
        if (player) {
            override_material(true);
            original(ecx, render_ctx, state, info, bone_to_world);
            ecx->forced_material_override(nullptr);
            override_material(false);
        }
    }

    original(ecx, render_ctx, state, info, bone_to_world);
    interfaces::model_render->forced_material_override(nullptr);
}

int __fastcall hooks::list_leaves_in_box::fn(se::spatial_query* ecx, int, const vec3& mins, const vec3& maxs, unsigned short* list, int list_max)
{
    static const auto insert_into_tree = memory::find_bytes(dll::client, PATTERN("89 44 24 14 EB 08 C7 44 24 ? ? ? ? ? 8B 45"));

    constexpr float max_coord = 16384.0f;
    constexpr vec3 new_maxs = { max_coord, max_coord, max_coord };
    constexpr vec3 new_mins = { -max_coord, -max_coord, -max_coord };

    if (_ReturnAddress() != insert_into_tree)
        return original(ecx, mins, maxs, list, list_max);

    const auto info = *reinterpret_cast<cs::renderable_info**>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14);
    const auto entity = info->renderable->get_client_unknown()->get_base_entity();
    if (!entity)
        return original(ecx, mins, maxs, list, list_max);

    // Force into translucent group.
    if (entity->is_player()) {
        info->flags.unset(cs::render_flag::force_opaque_pass);
        info->flags.set(cs::render_flag::bounds_always_recompute); // Setting IS_SPRITE (0x80) is unnecessary
    }

    // Return with largest possible world bounds to draw player models over longer distances
    return original(ecx, new_mins, new_maxs, list, list_max);
}

void __fastcall hooks::lock_cursor::fn(se::surface* ecx, int)
{
    return menu::is_active ? ecx->unlock_cursor() : original(ecx);
}

void __fastcall hooks::on_add_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle) {

    auto unknown = reinterpret_cast<cs::unknown*>(handle_entity);
    if (unknown; auto entity = unknown->get_base_entity())
        cache::add(entity);

    return original(ecx, handle_entity, handle);

}

void __fastcall hooks::on_remove_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle) {

    auto unknown = reinterpret_cast<cs::unknown*>(handle_entity);
    if (unknown; auto entity = unknown->get_base_entity())
        cache::remove(entity);

    return original(ecx, handle_entity, handle);

}

void hooks::spotted::proxy(cs::recv_proxy_data* data, void* arg0, void* arg1)
{
    if (config::get<bool>(vars::radar_reveal))
        data->value.integer = 1;

    return original(data, arg0, arg1);
}

#undef SET_SIG_HOOK
#undef SET_VT_HOOK
#undef SET_IAT_HOOK
