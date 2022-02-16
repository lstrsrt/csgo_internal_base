#pragma once

#include "../../memory/memory.h"
#include "client_class.h"

namespace cs {

struct event_info {
    cs::class_id class_id{ };
    float fire_delay{ };
    PAD(0x4)
    client_class* client_class{ };
    void* data{ };
    PAD(0x24)
    event_info* next{ };
};

struct game_event {
    VIRTUAL_FUNCTION(get_name, const char*, 1, (), (this))
    VIRTUAL_FUNCTION(get_bool, bool, 5, (const char* name, bool default_value = false), (this, name, default_value))
    VIRTUAL_FUNCTION(get_int, int, 6, (const char* name, int default_value = 0), (this, name, default_value))
    VIRTUAL_FUNCTION(get_uint64, uint64_t, 7, (const char* name, unsigned long default_value = 0), (this, name, default_value))
    VIRTUAL_FUNCTION(get_float, float, 8, (const char* name, float default_value = 0.0f), (this, name, default_value))
    VIRTUAL_FUNCTION(get_string, const char*, 9, (const char* name, const char* default_value = ""), (this, name, default_value))
    VIRTUAL_FUNCTION(get_wstring, const wchar_t*, 10, (const char* name, const wchar_t* default_value = L""), (this, name, default_value))
    VIRTUAL_FUNCTION(get_ptr, const void*, 11, (const char* name, const void* default_value = nullptr), (this, name, default_value))
};

}
