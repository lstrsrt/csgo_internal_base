#pragma once

#include <minwindef.h>

#define DECLARE_VF_HOOK(name, ret, base, idx, ... /* args */) namespace name { \
using ty = ret(__thiscall*)(base* ecx, __VA_ARGS__); \
inline ty original; \
inline ret __fastcall fn(base* ecx, int, __VA_ARGS__); \
constexpr unsigned int index = idx; \
}

#define DECLARE_SIG_HOOK(name, ret, args) namespace name { \
using ty = ret(__stdcall*)##args; \
inline ty original; \
inline ret __stdcall fn##args; \
}

#define DECLARE_PROXY(name, prop_name) namespace name { \
inline cs::recv_proxy_fn original{ }; \
inline void proxy(cs::recv_proxy_data* data, void*, void*); \
}

namespace hooks {

    inline HWND game_window{ };
    inline WNDPROC original_wnd_proc{ };
    extern LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    DECLARE_VF_HOOK(create_move_proxy, void, se::client_dll, 22, int, float, bool)
    DECLARE_VF_HOOK(frame_stage_notify, void, se::client_dll, 37, cs::frame_stage)
    DECLARE_VF_HOOK(override_view, void, se::client_mode, 18, cs::view_setup*)
    DECLARE_VF_HOOK(get_viewmodel_fov, float, se::client_mode, 35)
    DECLARE_VF_HOOK(is_connected, bool, se::engine_client, 27)
    DECLARE_VF_HOOK(override_config, bool, se::material_system, 21, cs::material_system_config*, bool)
    DECLARE_VF_HOOK(draw_model_execute, void, se::model_render, 21, cs::mat_render_context*,
                    const cs::draw_model_state&, const cs::model_render_info&, mat3x4*)
    DECLARE_VF_HOOK(paint_traverse, void, se::panel, 41, cs::vpanel, bool, bool)
    DECLARE_VF_HOOK(list_leaves_in_box, int, se::spatial_query, 6, const vec3&, const vec3&, unsigned short*, int)
    DECLARE_VF_HOOK(lock_cursor, void, se::surface, 67)

    DECLARE_SIG_HOOK(cull_beam, int, (const vec3&, const vec3&, int))

    DECLARE_PROXY(spotted, "CBaseEntity->m_bSpotted")

}

#undef DECLARE_VF_HOOK
#undef DECLARE_SIG_HOOK
#undef DECLARE_PROXY
