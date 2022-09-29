#pragma once

#include "../cs/trace.h"

namespace cs {

struct dlight;
struct material;
struct player_info;

enum class game_type {
    unknown = -1,
    classic,
    gungame,
    training,
    custom,
    cooperative,
    skirmish,
    freeforall
};

enum class game_mode {
    unknown = 0,
    casual,
    competitive,
    wingman,
    arms_race,
    demolition,
    deathmatch,
    guardian,
    coopstrike,
    danger_zone
};

}

namespace steam { struct api_context; }

namespace se {

struct debug_overlay {
    VIRTUAL_FUNCTION(add_box_overlay, void, 1, (const vec3* origin, const vec3* mins, const vec3* maxs, const angle* angles, const clr4* clr,
        float duration), (this, origin, mins, maxs, angles, clr, duration))
    VIRTUAL_FUNCTION(add_line_overlay, void, 4, (const vec3* origin, const vec3* dest, const clr4& clr, bool no_depth_test, float duration),
        (this, origin, dest, clr, no_depth_test, duration))
    VIRTUAL_FUNCTION(screen_position, int, 13, (const vec3* point, vec3* screen), (this, point, screen))
    VIRTUAL_FUNCTION(add_capsule_overlay, void, 23, (const vec3* start, const vec3* end, float* radius, const clr4* clr, float duration,
        bool ignore_z), (this, start, end, radius, clr, duration, ignore_z))
};

struct effects {
    VIRTUAL_FUNCTION(alloc_dlight, cs::dlight*, 4, (int key), (this, key))
    VIRTUAL_FUNCTION(alloc_elight, cs::dlight*, 5, (int key), (this, key))
};

struct spatial_query;
struct net_channel_info;

struct engine_client {
    VIRTUAL_FUNCTION(get_screen_size, void, 5, (int& width, int& height), (this, std::ref(width), std::ref(height)))
    VIRTUAL_FUNCTION(get_player_info, bool, 8, (int entity_index, cs::player_info* player_info), (this, entity_index, player_info))
    VIRTUAL_FUNCTION(get_player_for_uid, int, 9, (int uid), (this, uid))
    VIRTUAL_FUNCTION(get_local_player, int, 12, (), (this))
    VIRTUAL_FUNCTION(get_view_angles, void, 18, (angle& angles), (this, std::ref(angles)))
    VIRTUAL_FUNCTION(set_view_angles, void, 19, (angle& angles), (this, std::ref(angles)))
    VIRTUAL_FUNCTION(get_max_clients, int, 20, (), (this))
    VIRTUAL_FUNCTION(is_in_game, bool, 26, (), (this))
    VIRTUAL_FUNCTION(is_connected, bool, 27, (), (this))
    VIRTUAL_FUNCTION(world_to_screen_matrix, const mat4x4&, 37, (), (this))
    VIRTUAL_FUNCTION(get_bsp_tree_query, spatial_query*, 43, (), (this))
    VIRTUAL_FUNCTION(get_level_name, const char*, 52, (), (this))
    VIRTUAL_FUNCTION(get_level_name_short, const char*, 53, (), (this))
    VIRTUAL_FUNCTION(get_net_channel_info, net_channel_info*, 78, (), (this))
    VIRTUAL_FUNCTION(is_paused, bool, 90, (), (this))
    VIRTUAL_FUNCTION(is_taking_screenshot, bool, 92, (), (this))
    VIRTUAL_FUNCTION(is_hltv, bool, 93, (), (this))
    VIRTUAL_FUNCTION(get_engine_build_number, unsigned int, 104, (), (this))
    VIRTUAL_FUNCTION(get_product_version_string, const char*, 105, (), (this))
    VIRTUAL_FUNCTION(exec_client_cmd, void, 114, (const char* cmd, bool from_console_or_keybind = false), // Unrestricted version
        (this, cmd, from_console_or_keybind))
    VIRTUAL_FUNCTION(get_steam_api_context, steam::api_context*, 185, (), (this))
    VIRTUAL_FUNCTION(is_voice_recording, bool, 224, (), (this))

    d2 get_screen_size() noexcept
    {
        int x{ }, y{ };
        get_screen_size(x, y);
        return d2{ x, y };
    }

    angle& get_view_angles() noexcept
    {
        angle ret{ };
        get_view_angles(ret);
        return ret;
    }
};

class engine_trace {
private:
    VIRTUAL_FUNCTION(trace_ray, void, 5, (const cs::ray& ray, cs::trace_mask_t contents_mask, const cs::trace_filter& filter,
        cs::trace& trace), (this, std::cref(ray), contents_mask, std::cref(filter), std::ref(trace)))

public:
    VIRTUAL_FUNCTION(get_point_contents, int, 0, (const vec3& pos, cs::trace_mask_t mask = cs::trace_mask::all,
        cs::base_entity** entity = nullptr), (this, std::cref(pos), mask, entity))
    VIRTUAL_FUNCTION(clip_ray_to_entity, void, 3, (const cs::ray& ray, cs::trace_mask_t mask, cs::base_entity* entity, cs::trace* trace),
        (this, std::cref(ray), mask, entity, trace))

    inline void trace_line(const vec3& src, const vec3& dest, cs::trace_mask_t contents_mask, cs::base_entity* entity,
        cs::collision_group group, cs::trace& trace, cs::should_hit_fn extra_should_hit_check = nullptr) noexcept
    {
        trace_ray(cs::ray(src, dest), contents_mask, cs::trace_filter_simple(entity, group, extra_should_hit_check), trace);
    }

};

struct event_listener {
    static constexpr int event_debug_id_init = 42;

    virtual ~event_listener() {}
    virtual void fire_game_event(cs::game_event* event) = 0;
    virtual int get_event_debug_id() { return event_debug_id_init; }
};

struct event_manager {
    VIRTUAL_FUNCTION(add_listener, bool, 3, (event_listener* listener, const char* name, bool server_side = false),
        (this, listener, name, server_side))
    VIRTUAL_FUNCTION(remove_listener, void, 5, (event_listener* listener), (this, listener))
};

struct game_types {
    VIRTUAL_FUNCTION(get_current_game_type, cs::game_type, 8, (), (this))
    VIRTUAL_FUNCTION(get_current_game_mode, cs::game_mode, 9, (), (this))
};

struct model_info {
    VIRTUAL_FUNCTION(get_model, cs::model*, 1, (int index), (this, index))
    VIRTUAL_FUNCTION(get_model_index, int, 2, (const char* name), (this, name))
    VIRTUAL_FUNCTION(get_model_name, const char*, 3, (const cs::model* model), (this, model))
    VIRTUAL_FUNCTION(get_studio_model, cs::studio_hdr_t*, 32, (const cs::model* model), (this, model))
};

struct model_render {
    VIRTUAL_FUNCTION(forced_material_override, void, 1, (cs::material* material, int override_type = 0, int overrides = 0),
        (this, material, override_type, overrides))
    VIRTUAL_FUNCTION(is_forced_material_override, bool, 2, (), (this))
};

struct string_table {
    VIRTUAL_FUNCTION(add_string, void, 8, (bool is_server, const char* value, int length = -1, const void* user_data = nullptr),
        (this, is_server, value, length, user_data))
};

struct string_table_container {
    VIRTUAL_FUNCTION(find_table, string_table*, 3, (const char* table_name), (this, table_name))
};

struct render_view {
    VIRTUAL_FUNCTION(set_blend, void, 4, (float blend), (this, blend))
    VIRTUAL_FUNCTION(set_color, void, 6, (float* value), (this, value))
};

struct sound;

struct vgui {
    VIRTUAL_FUNCTION(get_panel, cs::vpanel, 1, (cs::vgui_panel panel), (this, panel))
};

}
