#pragma once

#include "../../memory/memory.h"

#include "../cs/client_class.h"
#include "../cs/game_event.h"
#include "../cs/utl_vector.h"
#include "../cs/entity.h"

namespace cs {

struct base_player;
struct hud_chat;
struct user_cmd;

enum class hud_filter {
    print_notify, // Same as print_console
    print_console,
    print_talk,   // Chat + console
    print_center
};

}

namespace se {

struct client_dll {
    VIRTUAL_FUNCTION(get_all_classes, cs::client_class*, 8, (), (this))
    VIRTUAL_FUNCTION(dispatch_user_message, bool, 38, (int msg_type, int flags, int size, const void* msg = nullptr), 
        (this, msg_type, flags, size, msg))
};

struct client_input {
    PAD(0xc)
    bool is_track_ir_available{ };
    bool is_mouse_initialized{ };
    bool is_mouse_active{ };
    PAD(0x9e)
    bool is_camera_in_thirdperson{ };
    PAD(0x2)
    vec3 camera_offset{ };
    PAD(0x38)
    cs::user_cmd* cmds{ };
};

struct hud_chat;

struct client_mode {
    auto get_hud_chat() noexcept
    {
        return *reinterpret_cast<hud_chat**>(reinterpret_cast<uintptr_t>(this) + 0x1c);
    }
};

struct net_channel;

struct client_state {
    // CBaseClientState
    PAD(0x9c)
    net_channel* net_channel{ };
    unsigned int challenge_number{ };
    PAD(0x4)
    double connect_time{ };
    int retry_number{ };
    PAD(0x54)
    int signon_state{ };
    PAD(0x4)
    double next_cmd_time{ };
    int server_count{ };
    int cur_sequence{ };
    PAD(0x48)
    int server_tick{ };
    int client_tick{ };
    int delta_tick{ };
    PAD(0x4)
    int view_entity{ };
    int player_slot{ };
    bool is_paused{ };
    PAD(0x3)
    char level_name[MAX_PATH]{ };
    char level_name_short[40]{ };
    PAD(0xd4)
    int max_clients{ };
    PAD(0x4994)
    // CClientState
    int old_tick_count{ };
    float tick_remainder{ };
    float frame_time{ };
    int last_outgoing_cmd{ };
    int choked_cmds{ };
    int last_cmd_acknowledged{ };
    int last_server_tick{ };
    int cmd_acknowledged{ };
    int sound_sequence{ };
    int last_progress_percent{ };
    bool is_hltv{ };
    PAD(0x4b)
    angle view_angles{ };
    PAD(0xcc)
    cs::event_info* events{ };
};

struct effects_client {
    VIRTUAL_FUNCTION(smoke, void, 2, (const vec3& origin, int model_index, float scale, float framerate), 
        (this, std::cref(origin), model_index, scale, framerate))
    VIRTUAL_FUNCTION(sparks, void, 3, (const vec3& position, int magnitude = 1, int trail_length = 1, const vec3* direction = nullptr), 
        (this, std::cref(position), magnitude, trail_length, direction))
    VIRTUAL_FUNCTION(dust, void, 4, (const vec3& position, const vec3& direction, float size, float speed), 
        (this, std::cref(position), std::cref(direction), size, speed))
    VIRTUAL_FUNCTION(muzzle_flash, void, 5, (const vec3& origin, const vec3& angles, float scale, int type), 
        (this, std::cref(origin), std::cref(angles), scale, type))
    VIRTUAL_FUNCTION(metal_sparks, void, 6, (const vec3& position, const vec3& direction), (this, std::cref(position), std::cref(direction)))
    VIRTUAL_FUNCTION(energy_splash, void, 7, (const vec3& position, const vec3& direction, bool explosive = false), 
        (this, std::cref(position), std::cref(direction), explosive))
    VIRTUAL_FUNCTION(ricochet, void, 8, (const vec3& position, const vec3& direction), (this, std::cref(position), std::cref(direction)))
};

struct entity_list {
    template<class ty = cs::base_entity*>
    VIRTUAL_FUNCTION(get, ty, 3, (int index), (this, index))
    template<class ty = cs::base_entity*>
    VIRTUAL_FUNCTION(get_from_handle, ty, 4, (int handle), (this, handle))
    VIRTUAL_FUNCTION(get_highest_index, int, 6, (), (this))
};

struct game_console {
    VIRTUAL_FUNCTION(activate, void, 1, (), (this))
    VIRTUAL_FUNCTION(hide, void, 3, (), (this))
    VIRTUAL_FUNCTION(clear, void, 4, (), (this))
    VIRTUAL_FUNCTION(is_console_visible, bool, 5, (), (this))
};

struct game_movement {
    VIRTUAL_FUNCTION(process_movement, void, 1, (cs::base_player* player, cs::move_data* move_data), (this, player, move_data))
    VIRTUAL_FUNCTION(reset, void, 2, (), (this))
};

struct game_rules {
    NETVAR(is_warmup_period, bool, "CCSGameRulesProxy->m_bWarmupPeriod")
    NETVAR(is_freeze_period, bool, "CCSGameRulesProxy->m_bFreezePeriod")
    NETVAR(is_valve_ds, bool, "CCSGameRulesProxy->m_bIsValveDS")
    NETVAR(is_bomb_planted, bool, "CCSGameRulesProxy->m_bBombPlanted")
};

struct game_ui {
    VIRTUAL_FUNCTION(create_command_msg_box, void, 19, (const char* title, const char* message, bool show_ok = true, bool show_cancel = false,
        const char* ok_command = nullptr, const char* cancel_command = nullptr, const char* closed_command = nullptr, const char* legend = nullptr,
        const char* unknown = nullptr), (this, title, message, show_ok, show_cancel, ok_command, cancel_command, closed_command, legend, unknown))
};

struct global_vars_base {
    float real_time{ };
    int	frame_count{ };
    float absolute_frame_time{ };
    float cur_time{ };
    float frame_time{ };
    int max_clients{ };
    int tick_count{ };
    float interval_per_tick{ };
    float interpolation_amount{ };
    int sim_ticks_this_frame{ };
};

struct glow_manager {
    cs::utl_vec<cs::glow_object_definition> glow_objects{ };
    int first_free_slot{ };
};

struct hud_chat {
    template<class... va_args>
    void printf(cs::hud_filter filter, const char* fmt, va_args... args) noexcept
    {
        (*reinterpret_cast<void(***)(void*, cs::hud_filter, const char*, ...)>(this))[26](this, filter, fmt, args...);
    }

    template<class... va_args>
    void wprintf(cs::hud_filter filter, const wchar_t* fmt, va_args... args) noexcept
    {
        (*reinterpret_cast<void(***)(void*, cs::hud_filter, const wchar_t*, ...)>(this))[27](this, filter, fmt, args...);
    }
};

struct leaf_system;

struct move_helper {
    VIRTUAL_FUNCTION(set_host, void, 1, (cs::base_player* host), (this, host))
    VIRTUAL_FUNCTION(process_impacts, void, 4, (), (this))
};

struct player_resource {
    NETVAR(get_c4_carrier, int, "CCSPlayerResource->m_iPlayerC4")
    NETVAR(bombsite_a_pos, vec3, "CCSPlayerResource->m_bombsiteCenterA")
    NETVAR(bombsite_b_pos, vec3, "CCSPlayerResource->m_bombsiteCenterB")
};

struct prediction {
    VIRTUAL_FUNCTION(update, void, 3, (int start_frame, bool valid_frame, int incoming_acknowledged, int outgoing_command), 
        (this, start_frame, valid_frame, incoming_acknowledged, outgoing_command))
    VIRTUAL_FUNCTION(set_local_view_angles, void, 13, (const angle& angles), (this, std::cref(angles)))
    VIRTUAL_FUNCTION(setup_move, void, 20, (cs::base_player* player, cs::user_cmd* cmd, se::move_helper* helper, cs::move_data* move), 
        (this, player, cmd, helper, move))
    VIRTUAL_FUNCTION(finish_move, void, 21, (cs::base_player* player, cs::user_cmd* cmd, cs::move_data* move), (this, player, cmd, move))
    VIRTUAL_FUNCTION(check_moving_ground, void, 18, (cs::base_player* player, double frame_time), (this, player, frame_time))

    PAD(0x8)
    bool is_in_prediction{ };
    PAD(0x1)
    bool is_engine_paused{ };
    PAD(0xd)
    bool is_first_time_predicted{ };
};

struct render_beams {
    VIRTUAL_FUNCTION(draw_beam, void, 6, (cs::beam* beam), (this, beam))
    VIRTUAL_FUNCTION(create_beam_points, cs::beam*, 12, (cs::beam_info* beam_info), (this, beam_info))
};

struct view_render;

struct weapon_system {
    VIRTUAL_FUNCTION(get_weapon_data, cs::weapon_info*, 2, (short item_id), (this, item_id))
};

}
