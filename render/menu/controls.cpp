#include "controls.h"

void menu::controls::tab(std::wstring_view str, tab_id id) noexcept
{
    constexpr d2 region_size{ 90, 30 };
    const d2 pos{ base_pos.x + region_size.x * static_cast<int>(id), base_pos.y };
    clr4 text_clr{ 255, 255, 255, 255 };
    clr4 line_clr{ 30, 30, 30, 255 };

    if (input::is_hovering_item(pos, region_size)) {
        text_clr = hover_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            cur_tab = id;
    }

    if (id == cur_tab)
        line_clr = text_clr = accent_color;

    text(fonts::tab, { pos.x + static_cast<int>(region_size.x / 2), pos.y + static_cast<int>(region_size.y / 2) }, str,
         text_flag::centered, text_clr);
    line(pos.x + 1, pos.y + 1, pos.x + region_size.x + 1, pos.y + 1, line_clr);
}

void menu::controls::checkbox(std::wstring_view str, bool& var) noexcept
{
    constexpr d2 box_size{ 16, 16 };
    const d2 text_size = get_text_size(fonts::menu, str);
    const d2 pos = calculate_position(ctrl_type::checkbox, fnv1a::hash(str));
    clr4 fill_clr = inactive_color;

    outlined_rect(pos, box_size, clr4::black());

    if (var)
        fill_clr = accent_color;

    if (input::is_hovering_item(pos, { box_size.x + text_size.x, box_size.y })) {
        if (!var)
            fill_clr = hover_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            var = !var;
    }

    filled_rect(pos.x + 2, pos.y + 2, pos.x + box_size.x - 2, pos.y + box_size.y - 2, fill_clr);
    text(fonts::menu, { pos.x + box_size.x + 5, pos.y }, str);
}

void menu::controls::button(std::wstring_view str, std::function<void(void)> callback, d2 size) noexcept
{
    const d2 pos = calculate_position(ctrl_type::button, fnv1a::hash(str));
    clr4 fill_clr{ 30, 30, 30, 255 };
    clr4 text_clr{ 255, 255, 255, 255 };

    outlined_rect(pos, size);
    fill_clr = inactive_color;

    if (input::is_hovering_item(pos, size)) {
        text_clr = hover_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            callback();
    }

    filled_rect(pos.x + 1, pos.y + 1, pos.x + size.x - 1, pos.y + size.y + 2, fill_clr);
    text(fonts::menu, { pos.x + size.x / 2, pos.y + size.y / 2 }, str, text_flag::centered, text_clr);
}

void menu::controls::slider(std::wstring_view str, int& var, int min, int max) noexcept
{
    constexpr d2 region_size{ 120, 18 };
    const d2 pos = calculate_position(ctrl_type::slider, fnv1a::hash(str));
    const auto scaled = std::abs(static_cast<float>(min) / region_size.x - static_cast<float>(max) / region_size.x);
    const auto fill_width = var / scaled - (min / scaled);

    outlined_rect(pos.x, pos.y, pos.x + region_size.x, pos.y + region_size.y, clr4::black());
    filled_rect(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, inactive_color);
    filled_rect(pos.x + 2, pos.y + 2, pos.x + static_cast<int>(fill_width) - 2, pos.y + region_size.y - 2, accent_color);

    if (input::is_hovering_item(pos, region_size)) {
        if (holding_lmb())
            var = static_cast<int>((static_cast<float>(input::mouse_pos.x - pos.x)) / region_size.x * (max - min) + min);
    }

    text(fonts::menu, { pos.x + region_size.x / 2, pos.y + region_size.y / 2 }, std::to_wstring(var),
                 text_flag::centered);
    text(fonts::menu, { pos.x + region_size.x + 5, pos.y }, str);
}

void menu::controls::slider(std::wstring_view str, float& var, float min, float max, int precision) noexcept
{
    constexpr d2 region_size{ 120, 18 };
    const d2 pos = calculate_position(ctrl_type::slider, fnv1a::hash(str));
    const float scaled = std::abs(min / region_size.x - max / region_size.x);
    const float fill_width = var / scaled - (min / scaled);

    outlined_rect(pos.x, pos.y, pos.x + region_size.x, pos.y + region_size.y, clr4::black());
    filled_rect(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, inactive_color);
    filled_rect(pos.x + 2, pos.y + 2, pos.x + static_cast<int>(fill_width - 2), pos.y + region_size.y - 2, accent_color);

    if (input::is_hovering_item(pos, region_size)) {
        if (holding_lmb())
            var = static_cast<float>(static_cast<float>(input::mouse_pos.x - pos.x) / region_size.x * (max - min) + min);
    }

    std::wostringstream stream{ };
    stream << std::fixed << std::setprecision(precision) << var;

    text(fonts::menu, { pos.x + region_size.x / 2, pos.y + region_size.y / 2 }, stream.str(), text_flag::centered);
    text(fonts::menu, { pos.x + region_size.x + 5, pos.y }, str);
}

void menu::controls::multibox(std::wstring_view str, const std::vector<std::wstring_view>& names, std::vector<bool>& vars) noexcept
{
    constexpr d2 region_size{ 120, 18 };
    const d2 picker_size{ 100, static_cast<int>(region_size.y * vars.size()) };
    const d2 pos = calculate_position(ctrl_type::dropdown, fnv1a::hash(str));
    clr4 item_color = { 255, 255, 255, 255 };

    outlined_rect(pos, region_size, clr4::black());
    filled_rect(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, inactive_color);

    static bool is_active{ };
    if (input::is_hovering_item(pos, region_size)) {
        item_color = hover_color;
        if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
            is_active = !is_active;
    }

    bool empty{ true };
    std::wstring selected{ };
    int selected_amt{ };

    for (size_t i{ }; i < vars.size(); i++) {
        if (vars[i]) {
            empty = false;
            selected_amt++;
            if (i != 0 && selected_amt > 1)
                selected += L", ";
            selected += names[i];
            item_color = accent_color;
        } else
            item_color = { 255, 255, 255, 255 };

        if (is_active) {
            const int h = region_size.y * i;
            if (input::is_hovering_item({ pos.x, pos.y + static_cast<int>(region_size.y * (i + 1)) }, region_size)) {
                text(fonts::menu, { pos.x + 5, pos.y + region_size.y + h }, names[i], { }, hover_color);
                if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
                    vars[i] = !vars[i];
            } else
                text(fonts::menu, { pos.x + 5, pos.y + region_size.y + h }, names[i], { }, item_color);
        }
    }

    if (selected.size() > 22) {
        selected.resize(22);
        selected += L"...";
    }

    text(fonts::menu, { pos.x + 5, pos.y + 2 }, empty ? L"None" : selected);
    text(fonts::menu, { pos.x + region_size.x + 5, pos.y }, str);
}

d2 menu::controls::calculate_position(ctrl_type type, hash_t hash) noexcept
{
    /* TODO - when using multiple columns, x will not be constant anymore
       and one map for every column will be needed */

    constexpr size_t tab_count = static_cast<size_t>(tab_id::count);
    constexpr size_t ctrl_count = static_cast<size_t>(ctrl_type::count);

    constexpr std::array<int, ctrl_count> y_offsets{
        20, // Checkbox
        40, // Button
        20, // Slider
        20, // Dropdown
    };

    static std::array<std::unordered_map<hash_t, d2>, tab_count> all_items{ };
    static std::array<int, tab_count> total_y_offsets{ };
    static bool once = [] { std::ranges::fill(total_y_offsets, 34); return true; }();

    auto& items = all_items[static_cast<size_t>(cur_tab)];
    int& y_offset = total_y_offsets[static_cast<size_t>(cur_tab)];

    if (items.find(hash) == items.cend()) {
        items[hash] = { 10, y_offset };
        y_offset += static_cast<int>(y_offsets[static_cast<size_t>(type)]);
    }

    return menu::base_pos + items.at(hash);
}
