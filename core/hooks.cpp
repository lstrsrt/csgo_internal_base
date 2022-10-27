#include "cheat.h"
#include "../render/menu/menu.h"
#include "features/features.h"
#include "features/cache.h"
#include "features/variables.h"
#include "hooks.h"

void hooks::initialize() noexcept
{
    D3DDEVICE_CREATION_PARAMETERS creation_params{ };
    interfaces::dx9_device->GetCreationParameters(&creation_params);
    if (creation_params.hFocusWindow) {
        game_window = creation_params.hFocusWindow;
        original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLong(game_window, GWLP_WNDPROC, reinterpret_cast<LONG>(wnd_proc)));
    } else {
        LOG_ERROR("Invalid focus window, unable to redirect WndProc!");
    }

    hook_func = dlls::game_overlay_renderer.find(PATTERN("55 8B EC 51 8B 45 10 C7")).cast<decltype(hook_func)>();
    unhook_func = dlls::game_overlay_renderer.find(PATTERN("E8 ? ? ? ? 83 C4 08 FF 15")).absolute<decltype(unhook_func)>();

    SET_VT_HOOK(interfaces::client, level_shutdown, 7);
    SET_VT_HOOK(interfaces::client, frame_stage_notify, 37);
    SET_VT_HOOK(interfaces::client, create_move_proxy, 22);
    SET_VT_HOOK(interfaces::client_mode, override_view, 18);
    SET_VT_HOOK(interfaces::client_mode, get_viewmodel_fov, 35);
    SET_VT_HOOK(interfaces::vgui, paint, 14);
    SET_VT_HOOK(interfaces::model_render, draw_model_execute, 21);
    SET_VT_HOOK(interfaces::surface, lock_cursor, 67);

    SET_SIG_HOOK(dlls::client, "55 8B EC 51 8B 45 0C 53 56 8B F1 57", on_add_entity);
    SET_SIG_HOOK(dlls::client, "55 8B EC 51 8B 45 0C 53 8B D9 56 57 83 F8 FF 75 07", on_remove_entity);
    SET_SIG_HOOK(dlls::engine, "55 8B EC 83 E4 F8 83 EC 0C 8B C1", fire_event_intern);

    SET_PROXY("CBaseEntity->m_bSpotted", spotted);

    LOG_INFO("Hooks initialized.");

    cache::initialize();
}

void hooks::end() noexcept
{
    for (auto a : interfaces::hooked_tables)
        static_cast<interface_holder<void*>*>(a)->restore();

    unset(on_add_entity::fn);
    unset(on_remove_entity::fn);
    unset(fire_event_intern::fn);

    netvars::unset_proxy("CBaseEntity->m_bSpotted"_hash, spotted::original);

    SetWindowLongPtrA(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc));
}

LRESULT CALLBACK hooks::wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle }))
        menu::toggle();

    interfaces::input_system->enable_input(!menu::is_open);

    if (menu::is_open)
        return TRUE;

    return CallWindowProcA(original_wnd_proc, hwnd, msg, wparam, lparam);
}

void __fastcall hooks::level_shutdown::fn(se::client_dll* ecx, int)
{
    local.in_game = false;
    cache::clear();

    return original(ecx);
}

static void __stdcall create_move(int sequence_nr, float input_sample_time, bool is_active, bool& send_packet)
{
    hooks::create_move_proxy::original(interfaces::client, sequence_nr, input_sample_time, is_active);

    if (!interfaces::game_rules)
        interfaces::game_rules = **dlls::client.find(PATTERN("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A"))
        .offset(0x1).cast<se::game_rules***>();

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

    static auto override_postprocessing_disable = *dlls::client.find(PATTERN("83 EC 4C 80 3D")).offset(0x5).cast<bool**>();

    switch (frame_stage) {
    case cs::frame_stage::render_start:
        *override_postprocessing_disable = config::get<bool>(vars::disable_postprocessing);
        break;
    case cs::frame_stage::net_update_post_data_update_end:
        local.update();
        break;
    default:
        break;
    }

    return original(ecx, frame_stage);
}

void __fastcall hooks::override_view::fn(se::client_mode* ecx, int, cs::view_setup* view)
{
    if (!local || !local.in_game)
        return original(ecx, view);

    if (!local->is_scoping())
        view->fov = config::get<float>(vars::fov);

    return original(ecx, view);
}

float __fastcall hooks::get_viewmodel_fov::fn(se::client_mode* ecx, int)
{
    if (!local || (local && local->is_scoping()))
        return original(ecx);

    return config::get<float>(vars::viewmodel_fov);
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

    const auto model_name = interfaces::model_info->get_model_name(info.model);
    if (strstr(model_name, "models/player")) {
        const auto player = interfaces::entity_list->get<cs::player*>(info.entity_index);
        if (player && player->is_enemy()) {
            override_material({ 200, 0, 50, 255 }, true);
            original(ecx, render_ctx, state, info, bone_to_world);
            ecx->forced_material_override(nullptr);
            override_material({ 0, 255, 170, 255 }, false);
        }
    }

    original(ecx, render_ctx, state, info, bone_to_world);
    interfaces::model_render->forced_material_override(nullptr);
}

void __fastcall hooks::lock_cursor::fn(se::surface* ecx, int)
{
    return menu::is_open ? ecx->unlock_cursor() : original(ecx);
}

void __fastcall hooks::on_add_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle)
{
    if (ecx && handle_entity && handle.is_valid()) {
        auto unknown = static_cast<cs::unknown*>(handle_entity);
        if (unknown && unknown->get_base_entity())
            cache::add(unknown->get_base_entity());
    }

    return original(ecx, handle_entity, handle);
}

void __fastcall hooks::on_remove_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle)
{
    if (ecx && handle_entity && handle.is_valid()) {
        auto unknown = static_cast<cs::unknown*>(handle_entity);
        if (unknown && unknown->get_base_entity())
            cache::remove(unknown->get_base_entity());
    }

    return original(ecx, handle_entity, handle);
}

bool __fastcall hooks::fire_event_intern::fn(se::event_manager* ecx, int, cs::game_event* evt, bool server, bool client)
{
    if (!evt)
        return original(ecx, evt, server, client);

    switch (fnv1a::hash(evt->get_name())) {
    case "player_hurt"_hash:
        // auto attacker = cs::util::player_from_event(evt, "attacker");
        // ...
        break;
    }

    return original(ecx, evt, server, client);
}

void hooks::spotted::proxy(cs::recv_proxy_data* data, void* arg0, void* arg1)
{
    if (config::get<bool>(vars::radar_reveal))
        data->value.integer = 1;

    return original(data, arg0, arg1);
}

#undef SET_SIG_HOOK
#undef SET_VT_HOOK
