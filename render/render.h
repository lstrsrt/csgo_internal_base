#include "../base/math.h"
#include "../memory/interfaces.h"

namespace render {

    namespace fonts {
        inline h_font menu{ };
        inline h_font tab{ };
        inline h_font watermark{ };
        inline h_font esp{ };
    }

    enum class text_flag {
        none,
        centered_x = (1 << 0),
        centered_y = (1 << 1),
        centered = (centered_x | centered_y),
    };

    enum class flag {
        none,
        filled = (1 << 0),
        outlined = (1 << 1)
    };

    bool initialize() noexcept;

    h_font create_font(std::string_view name, int height, int weight, bitfield<font_flag> flags = { }) noexcept;
    d2 get_text_size(h_font font, std::wstring_view str) noexcept;
    d2 get_text_size(h_font font, std::string_view str) noexcept;
    void line(int x0, int y0, int x1, int y1, const clr4& clr = clr4::white()) noexcept;
    void line(d2 pos, d2 size, const clr4& clr = clr4::white()) noexcept;
    void filled_rect(int x0, int y0, int x1, int y1, const clr4& clr = clr4::white()) noexcept;
    void filled_rect(d2 pos, d2 size, const clr4& clr = clr4::white()) noexcept;
    void outlined_rect(int x0, int y0, int x1, int y1, const clr4& clr = clr4::white()) noexcept;
    void outlined_rect(d2 pos, d2 size, const clr4& clr = clr4::white()) noexcept;
    void outlined_circle(d2 center_pos, int radius, int segments = 12, const clr4& clr = clr4::white()) noexcept;
    void text(h_font font, d2 pos, std::wstring_view str, bitfield<text_flag> flags = { }, const clr4& clr = clr4::white()) noexcept;
    void text(h_font font, d2 pos, std::string_view str, bitfield<text_flag> flags = { }, const clr4& clr = clr4::white()) noexcept;

}
