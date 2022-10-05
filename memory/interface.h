#pragma once

#include <d3d9.h>

#include "../valve/se/se.h"
#include "../valve/cs/net.h"

namespace interfaces {

    inline std::vector<void*> hooked_tables{ };
    inline std::vector<std::pair<std::string, void*>> list{ };

}

template<class ptr> requires std::is_pointer_v<ptr>
struct interface_holder {
    ptr instance{ };
    uintptr_t* real_vmt{ };
    std::unique_ptr<uintptr_t[]> replacement_vmt{ };

    constexpr ptr operator->() noexcept { return instance; }
    constexpr operator ptr() noexcept { return instance; }
    constexpr ptr operator=(ptr rhs) noexcept { return instance; }

    // Pass false to replace_vmt if you don't hook anything from the table or if get_vmt_length() is crashing
    template<bool replace_vmt = false>
    inline void initialize(ptr vptr) noexcept
    {
        constexpr int dynamic_cast_info_len = 1;

        instance = vptr;

        if constexpr (replace_vmt) {
            real_vmt = *reinterpret_cast<uintptr_t**>(instance);

            const auto len = memory::get_vmt_length(real_vmt) + dynamic_cast_info_len;
            replacement_vmt = std::make_unique<uintptr_t[]>(len);
            std::ranges::copy(real_vmt - dynamic_cast_info_len, real_vmt + len - dynamic_cast_info_len, replacement_vmt.get());

            *reinterpret_cast<uintptr_t**>(instance) = replacement_vmt.get() + dynamic_cast_info_len;

            interfaces::hooked_tables.push_back(this);
        }
    }

    inline void restore() noexcept
    {
        *reinterpret_cast<uintptr_t**>(instance) = real_vmt;
    }
};

namespace interfaces {

inline interface_holder<se::client_dll*>             client{ };
inline interface_holder<se::client_input*>           input{ };
inline interface_holder<se::client_mode*>            client_mode{ };
inline interface_holder<se::client_state*>           client_state{ };
inline interface_holder<se::cvar*>                   cvar{ };
inline interface_holder<se::debug_overlay*>          debug_overlay{ };
inline interface_holder<se::effects_client*>         effects_client{ };
inline interface_holder<se::effects*>                effects{ };
inline interface_holder<se::engine_client*>          engine{ };
inline interface_holder<se::engine_trace*>           trace{ };
inline interface_holder<se::entity_list*>            entity_list{ };
inline interface_holder<se::event_manager*>          event_manager{ };
inline interface_holder<se::file_system*>            file_system{ };
inline interface_holder<se::game_console*>           console{ };
inline interface_holder<se::game_movement*>          movement{ };
inline interface_holder<se::game_rules*>             game_rules{ };
inline interface_holder<se::game_types*>             game_types{ };
inline interface_holder<se::game_ui*>                ui{ };
inline interface_holder<se::global_vars_base*>       globals{ };
inline interface_holder<se::glow_manager*>           glow_manager{ };
inline interface_holder<se::input_system*>           input_system{ };
inline interface_holder<se::leaf_system*>            leaf_system{ };
inline interface_holder<se::localize*>               localize{ };
inline interface_holder<se::match_framework*>        match_framework{ };
inline interface_holder<se::material_system*>        material_system{ };
inline interface_holder<se::mdl_cache*>              mdl_cache{ };
inline interface_holder<se::model_info*>             model_info{ };
inline interface_holder<se::model_render*>           model_render{ };
inline interface_holder<se::move_helper*>            move_helper{ };
inline interface_holder<se::net_channel*>            net_channel{ };
inline interface_holder<se::net_support*>            net_support{ };
inline interface_holder<se::panel*>                  panel{ };
inline interface_holder<se::player_resource*>        player_resource{ };
inline interface_holder<se::physics_surface_props*>  physics_props{ };
inline interface_holder<se::prediction*>             prediction{ };
inline interface_holder<se::render_beams*>           render_beams{ };
inline interface_holder<se::render_view*>            render_view{ };
inline interface_holder<se::server_dll*>             server{ };
inline interface_holder<se::sound*>                  sound{ };
inline interface_holder<se::spatial_query*>          bsp_query{ };
inline interface_holder<se::string_table_container*> network_string_table{ };
inline interface_holder<se::studio_render*>          studio_render{ };
inline interface_holder<se::surface*>                surface{ };
inline interface_holder<se::vgui*>                   vgui{ };
inline interface_holder<se::view_render*>            view_render{ };
inline interface_holder<se::weapon_system*>          weapon_system{ };

inline se::key_values_system*   key_values_system{ };
inline se::mem_alloc*           mem_alloc{ };

inline steam::api_context*      steam_api_ctx{ };
inline steam::client*           steam_client{ };
inline steam::game_coordinator* game_coordinator{ };
inline steam::friends*          steam_friends{ };
inline steam::matchmaking*      matchmaking{ };
inline steam::user_stats*       user_stats{ };

inline interface_holder<IDirect3DDevice9*> dx9_device{ };

void initialize() noexcept;

}
