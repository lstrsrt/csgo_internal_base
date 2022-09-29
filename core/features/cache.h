#pragma once

#include <ranges>

#include "../../valve/cs/entity.h"

namespace cs {

    enum class player_filter {
        dormant = 1,
        dead = 2,
        team = 4 /* Based on is_enemy(), not just the team id */
    };

    struct cached_entity {
        cs::base_entity* ptr;
        cs::entity_type type;
    };

    struct cached_player {
        cs::player* ptr;
        cs::entity_type type;
    };

}

namespace cache {

    inline std::vector<cs::cached_entity> entities{ };
    inline std::vector<cs::cached_player> players{ };

    inline void initialize() noexcept
    {
        if (!local.in_game)
            return;

        for (int i{ 1 }; i < interfaces::entity_list->get_highest_index(); i++) {
            if (auto entity = interfaces::entity_list->get(i))
                entities.emplace_back(entity);
        }
    }

    inline void add(cs::base_entity* entity) noexcept
    {
        const auto type = entity->get_entity_type();

        if (type == cs::entity_type::player)
            players.push_back(cs::cached_player(static_cast<cs::player*>(entity), type));
        else
            entities.push_back(cs::cached_entity(entity, type));
    }

    inline void remove(cs::base_entity* entity) noexcept
    {
        const auto type = entity->get_entity_type();

        if (type == cs::entity_type::player) {
            auto it = std::ranges::find_if(entities, [entity](cs::cached_entity& e) { return e.ptr == entity; });
            if (it != entities.cend())
                entities.erase(it);
        } else {
            auto it = std::ranges::find_if(players, [entity](cs::cached_player& e) { return e.ptr == static_cast<cs::player*>(entity); });
            if (it != players.cend())
                players.erase(it);
        }
    }

    inline void iterate_entities(std::invocable<cs::base_entity*> auto&& callback, bitfield<cs::entity_type> types = { } /* = all */) noexcept
    {
        auto view = entities | std::views::filter([unwanted = cs::entity_type::all & ~types.value()](const cs::cached_entity& e) {
            return (e.type & unwanted) == static_cast<cs::entity_type>(0);
        });

        for (auto& a : view)
            callback(a.ptr);
    }

    inline void iterate_players(std::invocable<cs::player*> auto&& callback, bitfield<cs::player_filter> filter) noexcept
    {
        for (auto& a : players) {
            auto* player = a.ptr;

            if (filter.is_set(cs::player_filter::dormant))
                if (player->is_dormant())
                    continue;
            if (filter.is_set(cs::player_filter::dead))
                if (!player->is_alive())
                    continue;
            if (filter.is_set(cs::player_filter::team))
                if (!player->is_enemy())
                    continue;

            callback(player);
        }
    }

}
