#include "menu.h"
#include "../../core/cheat.h"
#include "../../core/features/variables.h"
#include "controls.h"

void menu::run() noexcept
{
    using namespace controls;

    if (!is_open)
        return;

    handle_movement();

    /* Menu bounds */
    render::filled_rect({ base_pos.x, base_pos.y + 1 }, size, { 30, 30, 30, 255 });
    render::line(base_pos.x + 1, base_pos.y, base_pos.x + size.x - 1, base_pos.y, accent_color);

    tab(L"AIMBOT", tab_id::aimbot);
    tab(L"VISUALS", tab_id::visuals);
    tab(L"MISC", tab_id::misc);
    tab(L"SKINS", tab_id::skins);
    tab(L"CONFIG", tab_id::config);

    static float float_slider{ };
    static int int_slider{ };
    static bool check{ };
    static std::vector multi{ false, false, false, false };
    static int test{ };
    static int test2{ };

    switch (cur_tab) {
    case tab_id::aimbot:
        checkbox(L"Checkbox", check);
        slider(L"Int slider", int_slider, 0, 100);
        slider(L"Float slider", float_slider, -20.f, 20.f, 2);
        listbox(L"Listbox", { L"One", L"Two", L"Three" }, test);
        multibox(L"Multibox", { L"First", L"Second", L"Third", L"Fourth" }, multi);
        break;
    case tab_id::visuals:
        checkbox(L"Radar reveal", cfg::get<bool>(vars::radar_reveal));
        checkbox(L"ESP", cfg::get<bool>(vars::esp));
        slider(L"FOV", cfg::get<float>(vars::fov), 30.0f, 130.0f);
        slider(L"Viewmodel FOV", cfg::get<float>(vars::viewmodel_fov), 20.0f, 150.0f);
        checkbox(L"Disable postprocessing", cfg::get<bool>(vars::disable_postprocessing));
        break;
    case tab_id::misc:
        checkbox(L"Infinite crouch", cfg::get<bool>(vars::infinite_crouch));
        break;
    case tab_id::skins:
        break;
    case tab_id::config:
        button(L"Write default", []() { cfg::write(L"default.cfg"); }, { 100, 30 });
        button(L"Read default", []() { cfg::read(L"default.cfg"); }, { 100, 30 });
        button(L"Unhook", []() { cheat::should_unhook = true; }, { 55, 30 });
        break;
    default:
        break;
    }
}

void menu::draw_watermark() noexcept
{
    std::string watermark_str{ "hello" };

    /* Time */
    std::time_t time;
    std::tm tm;
    char tm_str[9];
    time = std::time(nullptr);
    localtime_s(&tm, &time);
    std::strftime(tm_str, sizeof(tm_str), "%T", &tm);

    watermark_str += " | ";
    watermark_str += tm_str;

    /* -insecure */
    static const bool insecure = []()
    {
        const auto peb = reinterpret_cast<PEB*>(__readfsdword(0x30));
        return wcsstr(peb->ProcessParameters->CommandLine.Buffer, L"-insecure");
    }();

    if (insecure)
        watermark_str += " | insecure";

    auto size = render::get_text_size(render::fonts::watermark, watermark_str);
    render::filled_rect({ 2, 2 }, { size.x + 5, size.y + 4 }, clr4::black(150));
    render::text(render::fonts::watermark, { 5, 5 }, watermark_str);
}

void menu::handle_movement() noexcept
{
    static bool in_move{ };
    static d2 last_pos{ };

    if (in_move)
        base_pos += (input::mouse_pos - last_pos);

    if (input::is_hovering_item(base_pos, { size.x, 30 })) {
        if (controls::holding_lmb()) {
            in_move = true;
            last_pos = input::mouse_pos;
        } else if (in_move)
            in_move = false;
    }
}
