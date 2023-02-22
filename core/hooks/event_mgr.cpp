#include "../hooks.h"
#include "../features/features.h"

bool __fastcall hooks::fire_event_intern::fn(se::event_manager* ecx, int, cs::game_event* evt, bool server, bool client)
{
    if (!evt)
        return original(ecx, evt, server, client);

    switch (fnv1a::hash(evt->get_name())) {
    case "bullet_impact"_hash:
        if (evt->get_int("userid") != interfaces::engine->get_local_player())
            break;

        auto start = local->get_abs_origin() + local->get_view_offset();
        start.z -= 5.0f; // For better visibility
        auto end = vec3{ evt->get_float("x"), evt->get_float("y"), evt->get_float("z") };

        cs::beam_info beam_info{ start, end, { 100, 0, 200, 255 }, 0.0f, 2.5f, cs::beam_flag::fade_out };
        auto beam = interfaces::render_beams->create_beam_points(&beam_info);
        interfaces::render_beams->draw_beam(beam);
        break;
    }

    return original(ecx, evt, server, client);
}
