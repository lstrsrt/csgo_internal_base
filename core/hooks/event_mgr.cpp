#include "../hooks.h"

bool __fastcall hooks::fire_event_intern::fn(se::event_manager* ecx, int, cs::game_event* evt, bool server, bool client)
{
    if (!evt)
        return original(ecx, evt, server, client);

    switch (fnv1a::hash(evt->get_name())) {
    case "player_hurt"_hash:
        // auto attacker = cs::util::player_from_event(evt, "attacker");
        // ...
        break;
    }

    return original(ecx, evt, server, client);
}
