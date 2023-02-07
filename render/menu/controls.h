#pragma once

#include <functional>

#include "menu.h"

namespace menu::controls {

    using namespace render;

#pragma region internal
    d2 calculate_position(ctrl_type type, hash_t id) noexcept;

    inline bool holding_lmb() noexcept
    {
        return input::is_key_active({ VK_LBUTTON, input::key_type::hold });
    }
#pragma endregion

#pragma region items
    void tab(std::wstring_view str, tab_id id) noexcept;
    void checkbox(std::wstring_view str, bool& var) noexcept;
    void button(std::wstring_view str, std::function<void(void)> callback, d2 size) noexcept;
    void slider(std::wstring_view str, int& var, int min, int max) noexcept;
    void slider(std::wstring_view str, float& var, float min, float max, int precision = 0) noexcept;
    void multibox(std::wstring_view str, const std::vector<std::wstring_view>& names, std::vector<bool>& vars) noexcept;
    // void colorpicker(std::wstring_view str, clr3& clr) noexcept;
    // void colorpicker(std::wstring_view str, clr4& clr) noexcept;
    // void inputbox(std::wstring_view str, std::wstring& var) noexcept;
    // void keybinder(uint32_t& key) noexcept;

    template<class ty> requires std::integral<ty> || enumerator<ty>
    void listbox(std::wstring_view str, const std::vector<std::wstring_view>& names, ty& var) noexcept
    {
        constexpr d2 region_size{ 120, 18 };
        const d2 picker_size{ region_size.x, static_cast<int>(region_size.y * (1 + names.size())) };
        const d2 pos = calculate_position(ctrl_type::dropdown, fnv1a::hash(str));

        render::outlined_rect(pos, region_size, clr4::black());
        render::filled_rect(pos.x + 2, pos.y + 2, pos.x + region_size.x - 2, pos.y + region_size.y - 2, inactive_color);

        static bool is_active{ };
        if (input::is_hovering_item(pos, region_size)) {
            if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
                is_active = !is_active;
        }

        text(fonts::menu, { pos.x + 5, pos.y + 2 }, names[var]);
        text(fonts::menu, { pos.x + region_size.x + 5, pos.y }, str);

        if (is_active) {
            for (size_t i{ }; i < names.size(); i++) {
                const int h = region_size.y * i;
                if (input::is_hovering_item({ pos.x, pos.y + static_cast<int>(region_size.y * (i + 1)) }, region_size)) {
                    if (input::is_key_active({ VK_LBUTTON, input::key_type::toggle }))
                        var = static_cast<ty>(i);
                    text(fonts::menu, { pos.x + 2, pos.y + region_size.y + h }, names[i], { }, hover_color);
                } else {
                    text(fonts::menu, { pos.x + 2, pos.y + region_size.y + h }, names[i], { },
                                 var == i ? accent_color : clr4::white());
                }
            }
        }
    }
#pragma endregion

}
