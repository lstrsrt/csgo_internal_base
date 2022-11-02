#include "hooks.h"
#include "cheat.h"
#include "features/cache.h"
#include "../memory/hook_mgr.h"
#include "../memory/interfaces.h"

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

    SET_VT_HOOK(interfaces::client, level_init_post_entity, 6);
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

    for (auto& a : hooked_fns)
        unhook_func(a.second, false);

    netvars::unset_proxy("CBaseEntity->m_bSpotted"_hash, spotted::original);

    SetWindowLong(game_window, GWLP_WNDPROC, reinterpret_cast<LONG>(original_wnd_proc));
}
