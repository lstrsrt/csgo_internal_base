#pragma once

#include "../../memory/memory.h"

#include "utl_vector.h"

namespace cs {

enum class convar_flag {
    development_only = (1 << 1),
    cheat = (1 << 14)
};

struct command_base {
    VIRTUAL_FUNCTION(is_flag_set, bool, 2, (convar_flag flag), (this, flag))
    VIRTUAL_FUNCTION(add_flags, void, 3, (convar_flag flags), (this, flags))
    VIRTUAL_FUNCTION(remove_flags, void, 4, (convar_flag flags), (this, flags))
    VIRTUAL_FUNCTION(get_next, command_base*, 9, (), (this))
};

struct convar : public command_base {
    VIRTUAL_FUNCTION(get_name, const char*, 5, (), (this))
    VIRTUAL_FUNCTION(get_base_name, const char*, 6, (), (this))
    VIRTUAL_FUNCTION(get_float, float, 11, (), (this))
    VIRTUAL_FUNCTION(get_int, int, 12, (), (this))
    auto get_bool() noexcept
    {
        return !!get_int();
    }
    auto get_string() noexcept
    {
        const char* str = parent->string;
        return str ? str : "";
    }
    VIRTUAL_FUNCTION(set_value, void, 14, (const char* value), (this, value))
    VIRTUAL_FUNCTION(set_value, void, 15, (float value), (this, value))
    VIRTUAL_FUNCTION(set_value, void, 16, (int value), (this, value))

    PAD(0x4)
    convar* next{ };
    int registered{ };
    char* name{ };
    char* help_string{ };
    bitfield<convar_flag> flags{ };
    PAD(0x4)
    convar* parent{ };
    const char* default_value{ };
    char* string{ };
    int string_length{ };
    float float_value{ };
    int int_value;
    bool has_min{ };
    float min{ };
    bool has_max{ };
    float max{ };
    utl_vec<std::add_pointer_t<void(convar*, const char*, float)>> on_change_callbacks{ };
};

}
