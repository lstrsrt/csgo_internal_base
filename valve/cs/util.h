#pragma once

#include "../../memory/interface.h"

namespace cs {

namespace util {

    inline auto player_from_event(game_event* evt) noexcept
    {
        return interfaces::entity_list->get<cs::player*>(interfaces::engine->get_player_for_uid(evt->get_int("userid")));
    }

}

}
