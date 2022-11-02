#include "player.h"
#include "../../memory/interfaces.h"
#include "../../core/cheat.h"

namespace cs {

bool player::is_enemy(player* other) noexcept
{
    static const auto teammates_are_enemies = interfaces::cvar->find_var("mp_teammates_are_enemies");
    const auto cmp = other ? other : cheat::local.ptr;

    if (get_index() == cmp->get_index())
        return false;

    if (teammates_are_enemies->get_bool() && get_team() == cmp->get_team())
        return true;

    return get_team() != cmp->get_team();
}

bool player::get_info(player_info& info) noexcept
{
    return interfaces::engine->get_player_info(get_index(), &info);
}

bool local_player::update() noexcept
{
    in_game = interfaces::client_state->signon_state == cs::signon_state::full;
    if (!in_game)
        return false;

    if (!ptr)
        ptr = interfaces::entity_list->get<cs::player*>(interfaces::engine->get_local_player());

    if (!ptr)
        return false;

    return true;
}

void local_player::reset() noexcept
{
    ptr = nullptr;
    cur_cmd = nullptr;
    view = { };
    in_game = false;
}

}
