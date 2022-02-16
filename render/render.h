#include "../base/math.h"
#include "../memory/interface.h"

namespace render {

    namespace fonts {
        inline hfont menu{ };
        inline hfont tab{ };
        inline hfont watermark{ };
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
    
    void create_font(hfont& font, std::string_view name, int height, int weight, bitfield<font_flag> flags = { }) noexcept;
    void line(int x0, int y0, int x1, int y1, const clr4& clr = { 255, 255, 255, 255 }) noexcept;
    /* Rectangle must be filled or outlined. */
    void rectangle(int x0, int y0, int x1, int y1, bitfield<flag> flags = flag::outlined, const clr4& clr = { 255, 255, 255, 255 }) noexcept;
    void outlined_circle(const d2& center_pos, int radius, int segments = 12, const clr4& clr = { 255, 255, 255, 255 }) noexcept;
    void text(hfont font, const d2& pos, std::wstring_view str, bitfield<text_flag> flags = { }, const clr4& clr = { 255, 255, 255, 255 }) noexcept;

}
