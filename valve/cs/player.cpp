#include "player.h"
#include "../../memory/interface.h"
#include "../../core/cheat.h"

namespace cs {

bool player::is_enemy(player* other) noexcept
{
    static const auto teammates_are_enemies = interfaces::cvar->find_var("mp_teammates_are_enemies");
    if (teammates_are_enemies->get_int())
        return true;

    if (!other)
        return cheat::local_player->get_team() != get_team();

    if (other == this)
        return false;

    return other->get_team() != get_team();
}

bool local_player::update() noexcept
{
    cheat::local_player = interfaces::entity_list->get<cs::local_player*>(interfaces::engine->get_local_player());
    return cheat::local_player != nullptr;
}

}
