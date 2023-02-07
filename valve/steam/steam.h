#pragma once

#include "../../memory/memory.h"

namespace steam {

using h_pipe = uint32_t;
using h_user = uint32_t;

// Initialized in interface.cpp, use with client::get_generic_interface()
inline h_pipe pipe{ };
inline h_user user{ };

struct id {
    union {
        struct {
            uint32_t acc_id : 32;
            uint32_t acc_instance : 20;
            uint32_t acc_type : 4;
            int32_t universe : 8;
        };
        uint64_t full{ };
    };
};

enum class gc_result {
    ok,
    no_message,
    buffer_too_small,
    not_logged_on,
    invalid_message
};

struct client {
    VIRTUAL_FUNCTION(get_generic_interface, void*, 12, (h_user user, h_pipe pipe, const char* version), (this, user, pipe, version))
};

struct friends {
    VIRTUAL_FUNCTION(get_persona_name, const char*, 0, (), (this))
};

struct game_coordinator {
    VIRTUAL_FUNCTION(send_message, gc_result, 0, (uint32_t msg_type, const void* data, uint32_t cub_data), (this, msg_type, data, cub_data))
    VIRTUAL_FUNCTION(is_message_available, bool, 1, (uint32_t* cub_msg_size), (this, cub_msg_size))
    VIRTUAL_FUNCTION(retrieve_message, gc_result, 2, (uint32_t* msg_type, void* dest, uint32_t cub_dest, uint32_t* cub_msg_size),
        (this, msg_type, dest, cub_dest, cub_msg_size))
};

struct matchmaking {
    VIRTUAL_FUNCTION(create_lobby, int, 13, (int lobby_type, int max_members), (this, lobby_type, max_members))
    VIRTUAL_FUNCTION(join_lobby, int, 14, (id lobby_id), (this, lobby_id))
    VIRTUAL_FUNCTION(leave_lobby, void, 15, (id lobby_id), (this, lobby_id))
    VIRTUAL_FUNCTION(invite_user_to_lobby, bool, 16, (id lobby_id, id steam_invite_id), (this, lobby_id, steam_invite_id))
    VIRTUAL_FUNCTION(send_lobby_chat_msg, bool, 26, (id lobby_id, const void* msg_body, int size), (this, lobby_id, msg_body, size))
    VIRTUAL_FUNCTION(set_lobby_type, bool, 33, (id lobby_id, int lobby_type), (this, lobby_id, lobby_type))
    VIRTUAL_FUNCTION(set_lobby_joinable, bool, 34, (id lobby_id, bool joinable), (this, lobby_id, joinable))
    VIRTUAL_FUNCTION(set_lobby_owner, bool, 36, (id lobby_id, id new_owner), (this, lobby_id, new_owner))
};

struct user_stats {
    virtual bool request_current_stats() = 0;
    virtual bool get_stat(const char* name, int* data) = 0;
    virtual bool get_stat(const char* name, float* data) = 0;
    virtual bool set_stat(const char* name, int data) = 0;
    virtual bool set_stat(const char* name, float data) = 0;
    virtual bool update_average_rate_stat(const char* name, float count_this_session, double session_length) = 0;
    virtual bool get_achievement(const char* name, bool* achieved) = 0;
    virtual bool set_achievement(const char* name) = 0;
    virtual bool clear_achievement(const char* name) = 0;
    virtual bool get_achievement_and_unlock_time(const char* name, bool* achieved, uint32_t* unlock_time) = 0;
    virtual bool store_stats() = 0;
    virtual int get_achievement_icon(const char* name) = 0;
    virtual const char* get_achievement_display_attribute(const char* name, const char* key) = 0;
    virtual bool indicate_achievement_progress(const char* name, uint32_t current_progress, uint32_t max_progress) = 0;
    virtual uint32_t achievement_count() = 0;
    virtual const char* get_achievement_name(uint32_t achievement) = 0;
};

struct api_context {
    client* steam_client{ };
    void* steam_user{ };
    friends* steam_friends{ };
    void* steam_utils{ };
    matchmaking* steam_matchmaking{ };
    void* steam_game_search{ };
    user_stats* steam_user_stats{ };
    void* steam_apps{ };
    void* steam_matchmaking_servers{ };
    void* steam_networking{ };
    void* steam_remote_storage{ };
    void* steam_steam_screenshots{ };
    void* steam_http{ };
    void* controller{ };
    void* steam_ugc{ };
    void* steam_app_list{ };
    void* steam_music{ };
    void* steam_music_remote{ };
    void* steam_html_surface{ };
    void* steam_inventory{ };
    void* steam_video{ };
    void* steam_parental_settings{ };
    void* steam_input{ };
};

}
