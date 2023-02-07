#include "features.h"
#include "cache.h"
#include "../../base/math.h"
#include "../../render/render.h"

bool visuals::esp::generate_box(cs::base_entity* entity, bool is_player) noexcept
{
    const uint8_t alpha = is_player ? [entity]() -> uint8_t
    {
        if (entity->is_dormant() && static_cast<cs::player*>(entity)->is_alive()) {
            float delta = interfaces::globals->cur_time - entity->get_simulation_time();
            if (delta > 8.0f)
                return 0;

            if (delta > 4.0f) {
                float scalar = ((delta - 4.0f) * 100 / 4.0f) / 100;
                scalar = 1 - scalar;
                scalar = std::powf(scalar, 3);
                return static_cast<uint8_t>(scalar * 255);
            }
        }
        return 255;
    }() : 255;

    if (!alpha)
        return false;

    const vec3 min = entity->get_collideable()->obb_mins();
    const vec3 max = entity->get_collideable()->obb_maxs();

    std::array<vec3, 8> points{
        vec3{ min.x, min.y, min.z },
        vec3{ min.x, max.y, min.z },
        vec3{ max.x, max.y, min.z },
        vec3{ max.x, min.y, min.z },
        vec3{ max.x, max.y, max.z },
        vec3{ min.x, max.y, max.z },
        vec3{ min.x, min.y, max.z },
        vec3{ max.x, min.y, max.z }
    };

    int left = std::numeric_limits<int>::max();
    int top = left;
    int right = std::numeric_limits<int>::min();
    int bottom = right;

    const auto& coordinate_frame = entity->get_coordinate_frame();

    for (size_t i{ }; i < points.size(); i++) {
        auto res = math::world_to_screen(points[i].transform(coordinate_frame));
        if (!res.successful)
            return false;

        int x = static_cast<int>(res.screen.x);
        int y = static_cast<int>(res.screen.y);
        left = std::min(left, x);
        top = std::min(top, y);
        right = std::max(right, x);
        bottom = std::max(bottom, y);
    }

    box = { left, top, right, bottom, right - left, bottom - top, alpha };
    return true;
}

void visuals::esp::draw_box() noexcept
{
    render::outlined_rect({ box.left, box.top }, { box.width, box.height }, clr4::white(box.alpha));
    render::outlined_rect({ box.left - 1, box.top - 1 }, { box.width + 2, box.height + 2 }, clr4::black(box.alpha));
    render::outlined_rect({ box.left + 1, box.top + 1 }, { box.width - 2, box.height - 2 }, clr4::black(box.alpha));
}

void visuals::esp::draw_name(std::string_view name) noexcept
{
    const auto wname = std::wstring(name.begin(), name.end());
    render::text(render::fonts::esp, { box.left + box.width / 2, box.top - 12 },
                 wname, render::text_flag::centered_x, clr4::white(box.alpha));
}

void visuals::esp::draw_weapon(cs::weapon* weapon) noexcept
{
    if (!weapon)
        return;

    const auto info = weapon->get_info();
    if (!info)
        return;

    const auto wname = interfaces::localize->find_safe(info->weapon_name);
    render::text(render::fonts::esp, { box.left + box.width / 2, box.bottom },
                 wname, render::text_flag::centered_x, { 255, 255, 255, box.alpha });
}

void visuals::esp::draw_health(int health) noexcept
{
    const auto hp = std::clamp(health, 0, 100);
    const auto height = hp * box.height / 100;
    const auto r_g = static_cast<int>(2.55f * hp);

    render::outlined_rect({ box.left - 6, box.top - 1 }, { 4, box.height + 2 }, clr4::black(box.alpha));
    render::outlined_rect({ box.left - 5, box.bottom - height }, { 2, height }, clr4(255 - r_g, r_g, 0, box.alpha));

    if (hp == 100)
        return;

    auto hp_str = std::to_wstring(hp);
    auto hp_size = render::get_text_size(render::fonts::esp, hp_str);

    render::text(render::fonts::esp, { box.left - 4 - hp_size.x / 2, box.bottom - height - 8 },
                 hp_str, { }, clr4::white(box.alpha));
}

void visuals::esp::run() noexcept
{
    if (!cfg::get<bool>(vars::esp))
        return;

    if (!local || !local.in_game)
        return;

    cache::iterate_entities([](cs::base_entity* entity)
    {
        if (entity->get_owner_entity_handle().is_valid())
            return;

        if (!generate_box(entity, false))
            return;

        draw_box();
        draw_weapon(static_cast<cs::weapon*>(entity));
    }, cs::entity_type::weapon);

    cs::player_info info{ };
    cache::iterate_players([&info](cs::player* player)
    {
        if (!generate_box(player->get_base_entity(), true))
            return;

        draw_box();

        if (player->get_info(info))
            draw_name(info.name);

        draw_weapon(player->get_active_weapon());
        draw_health(player->get_health());
    }, cs::player_filter::dead | cs::player_filter::team);
}
