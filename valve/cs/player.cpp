#include "player.h"
#include "../../memory/interface.h"
#include "../../core/cheat.h"

namespace cs {

bool player::is_enemy(player* other) noexcept
{
    if (other == this)
        return false;

    static const auto teammates_are_enemies = interfaces::cvar->find_var("mp_teammates_are_enemies");
    if (teammates_are_enemies->get_int())
        return true;

    if (!other)
        return cheat::local->get_team() != get_team();

    return other->get_team() != get_team();
}

bool local_player::update() noexcept
{
    in_game = interfaces::client_state->signon_state == cs::signon_state::full;
    if (!in_game)
        return false;

    if (!local)
        local = interfaces::entity_list->get<cs::player*>(interfaces::engine->get_local_player());

    if (!local)
        return false;

    return true;
}

}
