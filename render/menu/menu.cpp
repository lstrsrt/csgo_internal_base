#include "menu.h"
#include "../../core/cheat.h"
#include "../../core/features/variables.h"

void menu::run() noexcept
{
    if (!is_open)
        return;

    // TODO - handle menu movement

    render::rectangle(position.x, position.y + 1, 600, 500, render::flag::filled, { 30, 30, 30, 255 });
    render::line(position.x + 1, position.y, 599, position.y, accent_color.to_clr4());

    tab(L"AIMBOT", tab_id::aimbot, { 90, 90 });
    tab(L"VISUALS", tab_id::visuals, { 180, 90 });
    tab(L"MISC", tab_id::misc, { 270, 90 });
    tab(L"SKINS", tab_id::skins, { 360, 90 });
    tab(L"CONFIG", tab_id::config, { 450, 90 });
    button(L"x", []() { toggle(); }, { 550, 90 }, { 50, 30 }, false);

    static int combo{ };

    switch (cur_tab) {
    case tab_id::aimbot:
        checkbox(L"Engine prediction", config::get<bool>(vars::prediction), { 100, 125 });
        combo_box(L"Example combo box", { L"Option 1", L"Option 2" }, combo, { 100, 145 });
        break;
    case tab_id::visuals:
        checkbox(L"Radar reveal", config::get<bool>(vars::radar_reveal), { 100, 125 });
        slider(L"FOV", config::get<float>(vars::fov), 30.0f, 130.0f, { 100, 145 }, 0);
        slider(L"Viewmodel FOV", config::get<float>(vars::viewmodel_fov), 30.0f, 130.0f, { 100, 165 }, 0);
        checkbox(L"Disable postprocessing", config::get<bool>(vars::disable_postprocessing), { 100, 185 });
        break;
    case tab_id::misc:
        button(L"Unhide convars", []() {
            int i{ };
            auto iter = interfaces::cvar->factory_internal_iterator();
            for (iter->set_first(); iter->is_valid(); iter->next()) {
                auto cmd = iter->get();
                if (cmd->is_flag_set(cs::convar_flag::development_only | cs::convar_flag::cheat)) {
                    cmd->remove_flags(cs::convar_flag::development_only | cs::convar_flag::cheat);
                    i++;
                }
            }
            LOG_INFO("Unhid {} convars.", i);
        }, { 100, 125 }, { 100, 30 });
        checkbox(L"Infinite crouch", config::get<bool>(vars::infinite_crouch), { 100, 160 });
        break;
    case tab_id::skins:
        break;
    case tab_id::config:
        button(L"Write default", []() { config::write(L"default.cfg"); }, { 100, 125 }, { 100, 30 });
        button(L"Read default", []() { config::read(L"default.cfg"); }, { 100, 160 }, { 100, 30 });
        button(L"Unhook", []() { cheat::should_unhook = true; }, { 100, 195 }, { 55, 30 });
        break;
    }
}
