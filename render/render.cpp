#include "render.h"

bool render::initialize() noexcept
{
    create_font(fonts::menu, "Consola", 15, 350, font_flag::outline);
    create_font(fonts::tab, "Tahoma", 12, 400, font_flag::outline);
    create_font(fonts::watermark, "Tahoma", 12, 300, font_flag::outline);
    LOG_INFO("Renderer initialized.");
    return true;
}

void render::create_font(hfont& font, std::string_view name, int height, int weight, bitfield<font_flag> flags) noexcept
{
    font = interfaces::surface->create_font();
    interfaces::surface->set_font_glyph_set(font, name.data(), height, weight, 0, 0, flags.raw());
}

void render::line(int x0, int y0, int x1, int y1, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_line(x0, y0, x1, y1);
}

void render::rectangle(int x0, int y0, int x1, int y1, bitfield<flag> flags, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);

    if (flags.is_set(flag::filled))
        interfaces::surface->draw_filled_rect(x0, y0, x1, y1);
    if (flags.is_set(flag::outlined))
        interfaces::surface->draw_outlined_rect(x0, y0, x1, y1);
}

void render::outlined_circle(const d2& center_pos, int radius, int segments, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_outlined_circle(center_pos.x, center_pos.y, radius, segments);
}

void render::text(hfont font, const d2& pos, std::wstring_view str, bitfield<text_flag> flags, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_text_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_set_text_font(font);

    auto text_pos = pos;
    if (!flags.is_empty()) {
        int x{ }, y{ };
        interfaces::surface->get_text_size(font, str.data(), &x, &y);
        if (flags.is_set(text_flag::centered_x))
            text_pos.x -= x / 2;
        if (flags.is_set(text_flag::centered_y))
            text_pos.y -= y / 2;
    }

    interfaces::surface->draw_set_text_pos(text_pos.x, text_pos.y);
    interfaces::surface->draw_print_text(str.data(), str.length());
}
