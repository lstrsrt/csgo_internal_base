#include "entity.h"
#include "../../memory/interfaces.h"

namespace cs {

entity_type base_entity::get_entity_type() noexcept
{
    if (is_player())
        return entity_type::player;

    if (is_weapon()) {
        auto wpn = reinterpret_cast<weapon*>(this);
        if (auto info = wpn->get_info()) {
            if (info->type == weapon_type::grenade)
                return entity_type::grenade;
            if (info->type == weapon_type::c4)
                return entity_type::bomb;
            if (info->type == weapon_type::grenade)
                return entity_type::grenade;
        }
    }

    return entity_type::other;
}

weapon_info* weapon::get_info() noexcept
{
    return interfaces::weapon_system->get_weapon_data(get_item_id());
}

float planted_c4::get_remaining_time() noexcept
{
    return std::clamp(get_blow_time() - interfaces::globals->cur_time, 0.0f, get_total_time());
}

float planted_c4::get_remaining_defuse_time() noexcept
{
    return std::clamp(get_defuse_countdown() - interfaces::globals->cur_time, 0.0f, get_defuse_length());
}

}
