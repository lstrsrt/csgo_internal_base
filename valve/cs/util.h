#pragma once

#include <functional>

#include "../../memory/interfaces.h"

namespace cs {

namespace util {

    inline auto player_from_event(game_event* evt, std::string_view name = "userid") noexcept
    {
        return interfaces::entity_list->get<player*>(interfaces::engine->get_player_for_uid(evt->get_int(name.data())));
    }

    inline bool is_targetable_player(base_entity* entity) noexcept
    {
        if (!entity || !entity->is_player())
            return false;

        const auto player = reinterpret_cast<cs::player*>(entity);
        return player->is_alive() && !player->is_dormant() && !player->is_immune();
    }

}

}
