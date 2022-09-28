#pragma once

namespace menu {

    enum class tab_id {
        aimbot,
        visuals,
        misc,
        skins,
        config
    };

    inline namespace controls {

        void tab(std::wstring_view text, tab_id id, const d2& pos) noexcept;
        void checkbox(std::wstring_view text, bool& var, const d2& pos) noexcept;
        void button(std::wstring_view text, std::function<void(void)> on_click, const d2& pos, const d2& size, bool bg = true) noexcept;
        void slider(std::wstring_view text, int& var, int min, int max, const d2& pos) noexcept;
        void slider(std::wstring_view text, float& var, float min, float max, const d2& pos, int precision) noexcept;
        void combo_box(std::wstring_view text, const std::vector<std::wstring_view>& options, int& var, const d2& pos) noexcept;
        // void clr3_picker(std::wstring_view text, clr3& clr, const d2& pos) noexcept;
        // void clr4_picker(std::wstring_view text, clr4& clr, const d2& pos) noexcept;
        // template<class ty>
        // void multi_combo_box(std::wstring_view text, bitfield<ty>& vars, const d2& pos) noexcept;
        // void list_box()
        // void input()
        // void key_input()

    }
}
