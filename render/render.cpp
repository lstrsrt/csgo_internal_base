#include "render.h"

bool render::initialize() noexcept
{
    fonts::menu = create_font("Tahoma", 13, 350, font_flag::outline);
    fonts::tab = create_font("Verdana", 13, 350, font_flag::antialias);
    fonts::watermark = create_font("Tahoma", 12, 300, font_flag::outline);
    fonts::esp = create_font("Verdana", 12, 350, cs::font_flag::dropshadow | cs::font_flag::antialias);

    LOG_INFO("Renderer initialized.");
    return true;
}

h_font render::create_font(std::string_view name, int height, int weight, bitfield<font_flag> flags) noexcept
{
    auto font = interfaces::surface->create_font();
    interfaces::surface->set_font_glyph_set(font, name.data(), height, weight, 0, 0, flags.raw());
    return font;
}

d2 render::get_text_size(h_font font, std::wstring_view str) noexcept
{
    d2 ret{ };
    interfaces::surface->get_text_size(font, str.data(), &ret.x, &ret.y);
    return ret;
}

d2 render::get_text_size(h_font font, std::string_view str) noexcept
{
    return get_text_size(font, std::wstring(str.begin(), str.end()));
}

void render::line(int x0, int y0, int x1, int y1, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_line(x0, y0, x1, y1);
}

void render::line(d2 pos, d2 size, const clr4& clr) noexcept
{
    return line(pos.x, pos.y, pos.x + size.x, pos.y + size.y, clr);
}

void render::filled_rect(int x0, int y0, int x1, int y1, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_filled_rect(x0, y0, x1, y1);
}

void render::filled_rect(d2 pos, d2 size, const clr4& clr) noexcept
{
    return filled_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y, clr);
}

void render::outlined_rect(int x0, int y0, int x1, int y1, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_outlined_rect(x0, y0, x1, y1);
}

void render::outlined_rect(d2 pos, d2 size, const clr4& clr) noexcept
{
    return outlined_rect(pos.x, pos.y, pos.x + size.x, pos.y + size.y, clr);
}

void render::outlined_circle(d2 center_pos, int radius, int segments, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_outlined_circle(center_pos.x, center_pos.y, radius, segments);
}

void render::text(h_font font, d2 pos, std::wstring_view str, bitfield<text_flag> flags, const clr4& clr) noexcept
{
    interfaces::surface->draw_set_text_color(clr.r, clr.g, clr.b, clr.a);
    interfaces::surface->draw_set_text_font(font);

    auto text_pos = pos;
    if (!flags.is_empty()) {
        auto size = get_text_size(font, str);
        if (flags.is_set(text_flag::centered_x))
            text_pos.x -= size.x / 2;
        if (flags.is_set(text_flag::centered_y))
            text_pos.y -= size.y / 2;
    }

    interfaces::surface->draw_set_text_pos(text_pos.x, text_pos.y);
    interfaces::surface->draw_print_text(str.data(), str.length());
}

void render::text(h_font font, d2 pos, std::string_view str, bitfield<text_flag> flags, const clr4& clr) noexcept
{
    return text(font, pos, std::wstring(str.begin(), str.end()), flags, clr);
}
