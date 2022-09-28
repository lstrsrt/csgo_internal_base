#pragma once

namespace se {

struct localize {
    VIRTUAL_FUNCTION(find, wchar_t*, 11, (const char* token_name), (this, token_name))
    VIRTUAL_FUNCTION(find_safe, const wchar_t*, 12, (const char* token_name), (this, token_name))
    VIRTUAL_FUNCTION(ansi_to_unicode, int, 13, (const char* ansi, wchar_t* unicode, int unicode_buf_size_in_bytes),
        (this, ansi, unicode, unicode_buf_size_in_bytes))
    VIRTUAL_FUNCTION(unicode_to_ansi, int, 14, (const wchar_t* unicode, char* ansi, int ansi_buf_size), (this, unicode, ansi, ansi_buf_size))
};

}
