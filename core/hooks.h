#pragma once

#include <Windows.h>

#include "../valve/cs/cs.h"
#include "../valve/se/se.h"

#define DECLARE_HOOK(name, ret, base, ... /* args */) namespace name { \
using ty = ret(__thiscall*)(base* ecx, __VA_ARGS__); \
inline ty original; \
ret __fastcall fn(base* ecx, int, __VA_ARGS__); \
}

#define DECLARE_PROXY(name, prop_name) namespace name { \
inline cs::recv_proxy_fn original{ }; \
void proxy(cs::recv_proxy_data* data, void*, void*); \
}

namespace hooks {

    inline HWND game_window{ };
    inline WNDPROC original_wnd_proc{ };
    extern LRESULT CALLBACK wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);

    DECLARE_HOOK(level_init_post_entity, void, se::client_dll)
    DECLARE_HOOK(level_shutdown, void, se::client_dll)
    DECLARE_HOOK(create_move_proxy, void, se::client_dll, int, float, bool)
    DECLARE_HOOK(frame_stage_notify, void, se::client_dll, cs::frame_stage)
    DECLARE_HOOK(override_view, void, se::client_mode, cs::view_setup*)
    DECLARE_HOOK(get_viewmodel_fov, float, se::client_mode)
    DECLARE_HOOK(on_add_entity, void, se::entity_list, cs::handle_entity*, cs::base_handle)
    DECLARE_HOOK(on_remove_entity, void, se::entity_list, cs::handle_entity*, cs::base_handle)
    DECLARE_HOOK(fire_event_intern, bool, se::event_manager, cs::game_event*, bool, bool)
    DECLARE_HOOK(draw_model, void, se::studio_render, cs::draw_model_results*,
        const cs::draw_model_info&, mat3x4*, float*, float*, const vec3&, int)
    DECLARE_HOOK(lock_cursor, void, se::surface)
    DECLARE_HOOK(paint, void, se::vgui, cs::paint_mode)

    DECLARE_PROXY(spotted, "CBaseEntity->m_bSpotted")

}

#undef DECLARE_HOOK
#undef DECLARE_PROXY
