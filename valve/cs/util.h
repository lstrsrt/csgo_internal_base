#pragma once

#include <functional>

#include "../../memory/interface.h"

namespace cs {

namespace util {

    inline auto player_from_event(game_event* evt) noexcept
    {
        return interfaces::entity_list->get<player*>(interfaces::engine->get_player_for_uid(evt->get_int("userid")));
    }

    inline bool is_targetable_player(base_entity* entity) noexcept
    {
        if (!entity || !entity->is_player())
            return false;

        return !entity->is_dormant() && entity->is_alive();
    }

    enum class player_filter {
        dormant = 1,
        dead = 2,
        team = 4 /* Based on is_enemy(), not just the team id */
    };

    inline void iterate_entities(std::function<void(base_entity*)> callback) noexcept
    {
        for (int i = 1; i <= interfaces::entity_list->get_highest_index(); i++) {
            auto entity = interfaces::entity_list->get(i);
            if (!entity)
                continue;

            callback(entity);
        }
    }

    inline void iterate_players(std::function<void(player*)> callback, bitfield<player_filter> filter = { }) noexcept
    {
        for (int i = 1; i <= cs::max_players; i++) {
            auto entity = interfaces::entity_list->get(i);
            if (!entity || !entity->is_player())
                continue;

            auto player = static_cast<cs::player*>(entity);

            if (filter.is_set(player_filter::dormant))
                if (player->is_dormant())
                    continue;
            if (filter.is_set(player_filter::dead))
                if (!player->is_alive())
                    continue;
            if (filter.is_set(player_filter::team))
                if (!player->is_enemy())
                    continue;

            callback(player);
        }
    }

}

}
