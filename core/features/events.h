#pragma once

#include "features.h"

namespace events {

// Event list: https://wiki.alliedmods.net/Counter-Strike:_Global_Offensive_Events

struct custom_listener : public se::event_listener {
    void fire_game_event(cs::game_event* evt) noexcept override 
    {
        switch (fnv1a::rt(evt->get_name())) {
        case fnv1a::ct("bullet_impact"):
            // ...
            break;
        }
    }
};

inline custom_listener listener{ };

inline void initialize(const std::vector<std::string_view>& event_names) noexcept
{
    for (const auto& evt : event_names)
        interfaces::event_manager->add_listener(&listener, evt.data());

    LOG_INFO("Event listener initialized.");
}

inline void end() noexcept
{
    interfaces::event_manager->remove_listener(&listener);
}

}
