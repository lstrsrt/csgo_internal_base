#pragma once

#include <d3d9.h>

#include "../valve/se/se.h"
#include "../valve/cs/net.h"

template<class ty>
class c_interface {
public:
    ty instance{ };

public:
    uintptr_t* real_vmt{ };
    std::unique_ptr<uintptr_t[]> replacement_vmt{ };
    bool needs_restore{ };

    constexpr auto operator->() noexcept
    {
        return instance;
    }

    constexpr operator ty() noexcept
    {
        return instance;
    }

    constexpr auto operator=(ty rhs) noexcept
    {
        return instance;
    }

    // Pass false to replace_vmt if you don't hook anything from the table or if get_vmt_length() is crashing
    template<bool replace_vmt = true>
    inline void initialize(ty vptr) noexcept
    {
        constexpr int dynamic_cast_info_len = 1;

        instance = vptr;

        if constexpr (replace_vmt) {
            real_vmt = *reinterpret_cast<uintptr_t**>(instance);

            const auto len = memory::get_vmt_length(real_vmt) + dynamic_cast_info_len;
            replacement_vmt = std::make_unique<uintptr_t[]>(len);
            std::ranges::copy(real_vmt - dynamic_cast_info_len, real_vmt + len - dynamic_cast_info_len, replacement_vmt.get());

            *reinterpret_cast<uintptr_t**>(instance) = replacement_vmt.get() + 1;
            needs_restore = true;
        }
    }
    
    inline void restore() noexcept
    {
        if (needs_restore)
            *reinterpret_cast<uintptr_t**>(instance) = real_vmt;
    }
};

namespace interfaces {

inline std::vector<std::pair<std::string, void*>> list{ };

inline c_interface<se::client_dll*>             client{ };
inline c_interface<se::client_input*>           input{ };
inline c_interface<se::client_mode*>            client_mode{ };
inline c_interface<se::client_state*>           client_state{ };
inline c_interface<se::cvar*>                   cvar{ };
inline c_interface<se::debug_overlay*>          debug_overlay{ };
inline c_interface<se::effects_client*>         effects_client{ };
inline c_interface<se::effects*>                effects{ };
inline c_interface<se::engine_client*>          engine{ };
inline c_interface<se::engine_trace*>           trace{ };
inline c_interface<se::entity_list*>            entity_list{ };
inline c_interface<se::event_manager*>          event_manager{ };
inline c_interface<se::file_system*>            file_system{ };
inline c_interface<se::game_console*>           console{ };
inline c_interface<se::game_movement*>          movement{ };
inline c_interface<se::game_types*>             game_types{ };
inline c_interface<se::game_ui*>                ui{ };
inline c_interface<se::global_vars_base*>       globals{ };
inline c_interface<se::glow_manager*>           glow_manager{ };
inline c_interface<se::input_system*>           input_system{ };
inline c_interface<se::leaf_system*>            leaf_system{ };
inline c_interface<se::localize*>               localize{ };
inline c_interface<se::match_framework*>        match_framework{ };
inline c_interface<se::material_system*>        material_system{ };
inline c_interface<se::mdl_cache*>              mdl_cache{ };
inline c_interface<se::model_info*>             model_info{ };
inline c_interface<se::model_render*>           model_render{ };
inline c_interface<se::move_helper*>            move_helper{ };
inline c_interface<se::net_channel*>            net_channel{ };
inline c_interface<se::net_support*>            net_support{ };
inline c_interface<se::panel*>                  panel{ };
inline c_interface<se::player_resource*>        player_resource{ };
inline c_interface<se::physics_surface_props*>  physics_props{ };
inline c_interface<se::prediction*>             prediction{ };
inline c_interface<se::render_beams*>           render_beams{ };
inline c_interface<se::render_view*>            render_view{ };
inline c_interface<se::server_dll*>             server{ };
inline c_interface<se::sound*>                  sound{ };
inline c_interface<se::spatial_query*>          bsp_query{ };
inline c_interface<se::string_table_container*> network_string_table{ };
inline c_interface<se::studio_render_context*>  studio_render_ctx{ };
inline c_interface<se::surface*>                surface{ };
inline c_interface<se::vgui*>                   vgui{ };
inline c_interface<se::view_render*>            view_render{ };
inline c_interface<se::weapon_system*>          weapon_system{ };

inline se::key_values_system* key_values_system{ };
inline se::mem_alloc* mem_alloc{ };

inline steam::api_context*      steam_api_ctx{ };
inline steam::client*           steam_client{ };
inline steam::game_coordinator* game_coordinator{ };
inline steam::friends*          steam_friends{ };
inline steam::matchmaking*      matchmaking{ };
inline steam::user_stats*       user_stats{ };

inline c_interface<IDirect3DDevice9*> dx9_device{ };

void initialize() noexcept;

}
