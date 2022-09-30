#pragma once

namespace cs {

using hfont = unsigned long;

enum class font_draw_type {
    _default,
    nonadditive,
    additive
};

enum class font_flag {
    none,
    italic = (1 << 0),
    underline = (1 << 1),
    strikeout = (1 << 2),
    symbol = (1 << 3),
    antialias = (1 << 4),
    gaussian_blur = (1 << 5),
    rotary = (1 << 6),
    dropshadow = (1 << 7),
    additive = (1 << 8),
    outline = (1 << 9),
    custom = (1 << 10),
    bitmap = (1 << 11)
};

using vpanel = uint32_t;

enum paint_mode {
    paint_ui_panels = (1 << 0),
    paint_ingame_panels = (1 << 1)
};

enum class vgui_panel {
    root,
    game_ui_dll,
    client_dll,
    tools,
    ingame_screens,
    game_dll,
    client_dll_tools
};

}

using cs::hfont;
using cs::font_draw_type;
using cs::font_flag;

struct vertex {
    vec3 pos{ };
    vec3 tex_coord{ };
};

namespace se {

struct surface {
    VIRTUAL_FUNCTION(draw_set_color, void, 15, (int r, int g, int b, int a), (this, r, g, b, a))
    VIRTUAL_FUNCTION(draw_set_color, void, 15, (const clr4& clr), (this, clr))
    VIRTUAL_FUNCTION(draw_filled_rect, void, 16, (int x0, int y0, int x1, int y1), (this, x0, y0, x1, y1))
    VIRTUAL_FUNCTION(draw_outlined_rect, void, 18, (int x0, int y0, int x1, int y1), (this, x0, y0, x1, y1))
    VIRTUAL_FUNCTION(draw_line, void, 19, (int x0, int y0, int x1, int y1), (this, x0, y0, x1, y1))
    VIRTUAL_FUNCTION(draw_poly_line, void, 20, (int* px, int* py, int points), (this, px, py, points))
    VIRTUAL_FUNCTION(draw_set_text_font, void, 23, (hfont font), (this, font))
    VIRTUAL_FUNCTION(draw_set_text_color, void, 25, (int r, int g, int b, int a), (this, r, g, b, a))
    VIRTUAL_FUNCTION(draw_set_text_pos, void, 26, (int x, int y), (this, x, y))
    VIRTUAL_FUNCTION(draw_print_text, void, 28, (const wchar_t* text, int len, font_draw_type draw_type = font_draw_type::_default),
        (this, text, len, draw_type))
    VIRTUAL_FUNCTION(draw_set_texture_rgba, void, 37, (int id, const unsigned char* rgba, int width, int height), (this, id, rgba, width, height))
    VIRTUAL_FUNCTION(draw_set_texture, void, 38, (int id), (this, id))
    VIRTUAL_FUNCTION(draw_textured_rect, void, 41, (int x0, int y0, int x1, int y1), (this, x0, y0, x1, y1))
    VIRTUAL_FUNCTION(create_new_texture_id, int, 43, (bool procedural = false), (this, procedural))
    VIRTUAL_FUNCTION(unlock_cursor, void, 66, (), (this))
    VIRTUAL_FUNCTION(create_font, hfont, 71, (), (this))
    VIRTUAL_FUNCTION(set_font_glyph_set, void, 72,
        (hfont font, const char* win_font_name, int height, int weight, int blur, int scanlines, int flags, int min_range = 0, int max_range = 0),
        (this, font, win_font_name, height, weight, blur, scanlines, flags, min_range, max_range))
    VIRTUAL_FUNCTION(get_text_size, void, 79, (hfont font, const wchar_t* text, int* width, int* height), (this, font, text, width, height))
    VIRTUAL_FUNCTION(play_sound, void, 82, (const char* filename), (this, filename))
    VIRTUAL_FUNCTION(draw_outlined_circle, void, 103, (int x, int y, int radius, int segments), (this, x, y, radius, segments))
    VIRTUAL_FUNCTION(draw_textured_polygon, void, 106, (int n, vertex* vertice, bool clip_vertices = true), (this, n, vertice, clip_vertices))
    VIRTUAL_FUNCTION(draw_colored_circle, void, 162, (int x, int y, float radius, int r, int g, int b, int a), (this, x, y, radius, r, g, b, a))

    VIRTUAL_FUNCTION_SIG(start_drawing, void, dll::vgui_mat_surface, "55 8B EC 83 E4 C0 83 EC 38", (this))
    VIRTUAL_FUNCTION_SIG(finish_drawing, void, dll::vgui_mat_surface, "8B 0D ? ? ? ? 56 C6 05", (this))
};

}
