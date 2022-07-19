#include "menu.h"

void menu::controls::checkbox(std::wstring_view text, bool& var, const d2& pos) noexcept
{
    constexpr int box_size = 15;
    clr3 fill_clr = inactive_ctrl_color;

    render::rectangle(pos.x, pos.y, pos.x + box_size, pos.y + box_size, render::flag::outlined, { 0, 0, 0, 255 });

    if (var)
        fill_clr = accent_color;

    if (input::is_hovering_item(pos, { box_size, box_size })) {
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            var = !var;
    }

    render::rectangle(pos.x + 2, pos.y + 2, pos.x + box_size - 2, pos.y + box_size - 2, render::flag::filled, fill_clr.to_clr4());
    render::text(render::fonts::menu, { pos.x + box_size + 5, pos.y }, text);
}

void menu::controls::tab(std::wstring_view text, tab_id id, const d2& pos) noexcept
{
    constexpr d2 region_size = { 90, 30 };
    clr3 text_clr = { 255, 255, 255 };
    clr3 line_clr = { 30, 30, 30 };

    if (input::is_hovering_item(pos, region_size)) {
        text_clr = accent_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            cur_tab = id;
    }

    if (cur_tab == id)
        line_clr = text_clr = accent_color;

    render::text(render::fonts::tab, { pos.x + 45, pos.y + 15 }, text, render::text_flag::centered, text_clr.to_clr4());
    render::line(pos.x + 1, pos.y + 1, pos.x + region_size.x + 1, pos.y + 1, line_clr.to_clr4());
    render::line(pos.x + 1, pos.y + 2, pos.x + region_size.x + 1, pos.y + 2, line_clr.to_clr4());
}

void menu::controls::button(std::wstring_view text, std::function<void(void)> on_click, const d2& pos, const d2& size, bool bg) noexcept
{
    clr3 fill_clr = { 30, 30, 30 };

    if (bg) {
        render::rectangle(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
        fill_clr = inactive_ctrl_color;
    }

    if (input::is_hovering_item(pos, size)) {
        if (!bg)
            fill_clr = accent_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            on_click();
    }

    render::rectangle(pos.x + 1, pos.y + 1, pos.x + size.x - 1, pos.y + size.y + 2, render::flag::filled, fill_clr.to_clr4());
    render::text(render::fonts::menu, { pos.x + size.x / 2, pos.y + size.y / 2 }, text, render::text_flag::centered);
}

void menu::controls::slider(std::wstring_view text, int& var, int min, int max, const d2& pos) noexcept
{
    constexpr d2 region_size = { 100, 18 }; // 125?
    const float scaled = abs(static_cast<float>(min) / static_cast<float>(region_size.x) - static_cast<float>(max) / static_cast<float>(region_size.x));
    // FIXME: doesn't work when min is negative and max is positive
    const int fill_width = std::clamp(region_size.x / (max - min) * (var - min), 0, region_size.x);
    
    render::rectangle(pos.x, pos.y, pos.x + region_size.x, pos.y + region_size.y, render::flag::outlined, { 0, 0, 0, 255 });
    render::rectangle(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, render::flag::filled, inactive_ctrl_color.to_clr4());
    render::rectangle(pos.x + 2, pos.y + 2, pos.x + fill_width - 2, pos.y + region_size.y - 2, render::flag::filled, accent_color.to_clr4());

    if (input::is_hovering_item(pos, region_size)) {
        if (input::is_key_active({ VK_LBUTTON, input::key_type::hold }))
            var = std::clamp(min + static_cast<int>((input::mouse_pos.x - pos.x) * scaled), min, max);
    }
    
    render::text(render::fonts::menu, { pos.x + region_size.x / 2, pos.y + region_size.y / 2 }, std::to_wstring(var), render::text_flag::centered);
    render::text(render::fonts::menu, { pos.x + region_size.x + 5, pos.y }, text);
}

void menu::controls::slider(std::wstring_view text, float& var, float min, float max, const d2& pos, int precision) noexcept
{
    constexpr d2 region_size = { 100, 18 }; // 125?
    const float scalar = (max - min) / region_size.x;
    const float fill_width = var / scalar - (min / scalar);

    render::rectangle(pos.x, pos.y, pos.x + region_size.x, pos.y + region_size.y, render::flag::outlined, { 0, 0, 0, 255 });
    render::rectangle(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, render::flag::filled, inactive_ctrl_color.to_clr4());
    render::rectangle(pos.x + 2, pos.y + 2, pos.x + static_cast<int>(fill_width - 2), pos.y + region_size.y - 2, render::flag::filled, accent_color.to_clr4());

    if (input::is_hovering_item(pos, region_size)) {
        if (input::is_key_active({ VK_LBUTTON, input::key_type::hold }))
            var = static_cast<int>((static_cast<float>(input::mouse_pos.x - pos.x)) / region_size.x * (max - min) + min);
    }

    std::wostringstream stream{ };
    stream << std::fixed << std::setprecision(precision) << var;

    render::text(render::fonts::menu, { pos.x + region_size.x / 2, pos.y + region_size.y / 2 }, stream.str(), render::text_flag::centered);
    render::text(render::fonts::menu, { pos.x + region_size.x + 5, pos.y }, text);
}

void menu::controls::combo_box(std::wstring_view text, const std::vector<std::wstring_view>& options, int& var, const d2& pos) noexcept
{

    // TODO - doesn't select anything yet

    constexpr d2 region_size = { 100, 18 };
    const d2 picker_size{ 100, static_cast<int>(18 * options.size()) };
    clr3 item_color = inactive_ctrl_color;

    render::rectangle(pos.x, pos.y, pos.x + region_size.x, pos.y + region_size.y, render::flag::outlined, { 0, 0, 0, 255 });
    render::rectangle(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, render::flag::filled, inactive_ctrl_color.to_clr4());
    render::line(pos.x + 85, pos.y - 2, pos.x + 85, pos.y + region_size.y - 2, { 0, 0, 0, 255 });
    // add triangle here...

    static bool is_selected{ };
    if (input::is_hovering_item(pos, region_size)) {
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            is_selected = !is_selected;
    }

    if (is_selected) {
        for (size_t i = 0; i < options.size(); i++) {
            render::text(render::fonts::menu, { pos.x + 2, pos.y + region_size.y + static_cast<int>(18 * i) }, options.at(i));
            if (input::is_hovering_item({ pos.x, pos.y + static_cast<int>(18 * i) }, region_size))
                item_color = accent_color;
            render::rectangle(pos.x + 2, pos.y + region_size.y + 2, pos.x + 2, pos.y + region_size.y + static_cast<int>(18 * i), 
                render::flag::filled, item_color.to_clr4());
        }
    }

    render::text(render::fonts::menu, { pos.x + 5, pos.y + 2 }, std::to_wstring(var));
    render::text(render::fonts::menu, { pos.x + region_size.x + 5, pos.y }, text);
}
