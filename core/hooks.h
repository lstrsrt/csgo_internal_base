#pragma once

#include <minwindef.h>

#define DECLARE_HOOK(name, ret, base, ... /* args */) namespace name { \
using ty = ret(__thiscall*)(base* ecx, __VA_ARGS__); \
inline ty original; \
inline ret __fastcall fn(base* ecx, int, __VA_ARGS__); \
}

#define DECLARE_PROXY(name, prop_name) namespace name { \
inline cs::recv_proxy_fn original{ }; \
inline void proxy(cs::recv_proxy_data* data, void*, void*); \
}

namespace hooks {

    inline HWND game_window{ };
    inline WNDPROC original_wnd_proc{ };
    extern LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    DECLARE_HOOK(create_move_proxy, void, se::client_dll, int, float, bool)
    DECLARE_HOOK(frame_stage_notify, void, se::client_dll, cs::frame_stage)
    DECLARE_HOOK(override_view, void, se::client_mode, cs::view_setup*)
    DECLARE_HOOK(get_viewmodel_fov, float, se::client_mode)
    DECLARE_HOOK(is_connected, bool, se::engine_client)
    DECLARE_HOOK(paint, void, se::vgui, cs::paint_mode)
    DECLARE_HOOK(override_config, bool, se::material_system, cs::material_system_config*, bool)
    DECLARE_HOOK(draw_model_execute, void, se::model_render, cs::mat_render_context*,
                 const cs::draw_model_state&, const cs::model_render_info&, mat3x4*)
    DECLARE_HOOK(list_leaves_in_box, int, se::spatial_query, const vec3&, const vec3&,
                 unsigned short*, int)
    DECLARE_HOOK(lock_cursor, void, se::surface)

    DECLARE_HOOK(on_add_entity, void, se::entity_list, cs::handle_entity*, cs::base_handle)
    DECLARE_HOOK(on_remove_entity, void, se::entity_list, cs::handle_entity*, cs::base_handle)

    DECLARE_PROXY(spotted, "CBaseEntity->m_bSpotted")

}

#undef DECLARE_HOOK
#undef DECLARE_PROXY
