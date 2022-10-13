#pragma once

#include <ranges>

#include "../../valve/cs/entity.h"

namespace cs {

    enum class player_filter {
        dormant = (1 << 0),
        dead = (1 << 1),
        team = (1 << 2) /* Based on is_enemy(), not just the team id */
    };

    struct cached_entity {
        explicit cached_entity(base_entity* p, const entity_type t) noexcept
            : ptr(p), type(t) { }

        cs::base_entity* ptr;
        cs::entity_type type;
    };

}

namespace cache {

    inline std::vector<cs::cached_entity> entities{ };
    inline std::vector<cs::player*> players{ };

    inline void initialize() noexcept
    {
        if (!local.in_game)
            return;

        for (int i{ 1 }; i < interfaces::entity_list->get_highest_index(); i++) {
            if (auto entity = interfaces::entity_list->get(i)) {
                const auto type = entity->get_entity_type();
                if (entity->is_player())
                    players.emplace_back(static_cast<cs::player*>(entity));
                else
                    entities.emplace_back(cs::cached_entity(entity, type));
            }
        }
    }

    inline void add(cs::base_entity* entity) noexcept
    {
        const auto type = entity->get_entity_type();
        if (entity->is_player())
            players.emplace_back(static_cast<cs::player*>(entity));
        else
            entities.emplace_back(cs::cached_entity(entity, type));
    }

    inline void remove(cs::base_entity* entity) noexcept
    {
        const auto type = entity->get_entity_type();
        if (entity->is_player()) {
            auto it = std::ranges::find_if(entities, [entity](cs::cached_entity& e) {
                return e.ptr == entity;
            });
            if (it != entities.cend())
                entities.erase(it);
        } else {
            auto it = std::ranges::find_if(players, [entity](cs::player* p) {
                return p == static_cast<cs::player*>(entity);
            });
            if (it != players.cend())
                players.erase(it);
        }
    }

    void iterate_entities(std::invocable<cs::base_entity*> auto&& callback,
                          bitfield<cs::entity_type> types = { } /* = all */) noexcept
    {
        if (entities.empty())
            return;

        if (types.is_empty()) {
            for (auto& a : entities)
                callback(a.ptr);
        } else {
            auto view = entities | std::views::filter([t = types.value()](const cs::cached_entity& e) {
                return (e.type & t) != static_cast<cs::entity_type>(0);
            });
            for (auto& a : view)
                callback(a.ptr);
        }
    }

    void iterate_players(std::invocable<cs::player*> auto&& callback,
                         bitfield<cs::player_filter> filter) noexcept
    {
        if (players.empty())
            return;

        for (auto& [player, type] : players) {
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
