#pragma once

#include <ranges>

#include "../../valve/cs/entity.h"
#include "../../valve/cs/player.h"
#include "features.h"

namespace cs {

    enum class player_filter {
        dormant = (1 << 0),
        dead = (1 << 1),
        team = (1 << 2) /* Based on is_enemy(), not just the team id */
    };

    struct cached_entity {
        explicit cached_entity(base_entity* ptr, entity_type type) noexcept
            : ptr(ptr), type(type) { }

        cs::base_entity* ptr;
        cs::entity_type type;
    };

}

namespace cache {

    inline std::vector<cs::cached_entity> entities{ };

    inline void clear() noexcept
    {
        entities.clear();
    }

    inline void initialize() noexcept
    {
        if (!interfaces::engine->is_in_game())
            return;

        clear();

        for (int i{ 1 }; i < interfaces::entity_list->get_highest_index(); i++) {
            if (auto entity = interfaces::entity_list->get(i))
                entities.emplace_back(cs::cached_entity(entity, entity->get_entity_type()));
        }
    }

    inline void add(cs::base_entity* entity) noexcept
    {
        const auto type = entity->get_entity_type();
        entities.emplace_back(cs::cached_entity(entity, type));
    }

    inline void remove(cs::base_entity* entity) noexcept
    {
        auto it = std::ranges::find_if(entities, [entity](auto& e) { return e.ptr == entity; });
        if (it != entities.cend())
            entities.erase(it);
    }

    void iterate_entities(std::invocable<cs::base_entity*> auto&& callback,
                          bitfield<cs::entity_type> types = { } /* = all */) noexcept
    {
        if (entities.empty())
            return;

        if (types.is_empty()) {
            for (auto& a : entities)
                callback(a.ptr);

            return;
        }

        auto view = entities | std::views::filter([t = types.value()](const cs::cached_entity& e) {
            return (e.type & t) != static_cast<cs::entity_type>(0);
        });

        for (auto& a : view)
            callback(a.ptr);
    }

    void iterate_players(std::invocable<cs::player*> auto&& callback,
                         bitfield<cs::player_filter> filter = { } /* = all */) noexcept
    {
        iterate_entities([callback, filter](cs::base_entity* entity)
        {
            auto player = reinterpret_cast<cs::player*>(entity);

            if (filter.is_set(cs::player_filter::dormant))
                if (player->is_dormant())
                    return;
            if (filter.is_set(cs::player_filter::dead))
                if (!player->is_alive())
                    return;
            if (filter.is_set(cs::player_filter::team))
                if (!player->is_enemy())
                    return;

            callback(player);
        }, cs::entity_type::player);
    }

}
