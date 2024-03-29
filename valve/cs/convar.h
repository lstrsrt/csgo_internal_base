#pragma once

#include "../../memory/memory.h"

#include "utl_vector.h"

namespace cs {

enum class convar_flag {
    development_only = (1 << 1),
    cheat = (1 << 14)
};

struct convar_value {
    char* string{ };
    int string_length{ };
    float float_value{ };
    int int_value;
};

struct command_base {
    VIRTUAL_FUNCTION(is_flag_set, bool, 2, (convar_flag flag), (this, flag))
    VIRTUAL_FUNCTION(add_flags, void, 3, (convar_flag flags), (this, flags))
    VIRTUAL_FUNCTION(remove_flags, void, 4, (convar_flag flags), (this, flags))
    VIRTUAL_FUNCTION(get_next, command_base*, 9, (), (this))
};

struct convar : command_base {
    VIRTUAL_FUNCTION(get_name, const char*, 5, (), (this))
    VIRTUAL_FUNCTION(get_base_name, const char*, 6, (), (this))

    auto get_float() noexcept
    {
        auto xored = *reinterpret_cast<uintptr_t*>(&parent->original.float_value) ^ reinterpret_cast<uintptr_t>(this);
        return *reinterpret_cast<float*>(&xored);
    }

    auto get_int() noexcept
    {
        return static_cast<int>(parent->original.int_value ^ reinterpret_cast<uintptr_t>(this));
    }

    auto get_bool() noexcept
    {
        return !!get_int();
    }

    auto get_string() noexcept
    {
        return parent->original.string ? parent->original.string : "";
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
    char* default_value{ };
    convar_value original{ };
    convar_value backup{ };
    int has_min{ };
    float min{ };
    int has_max{ };
    float max{ };
    utl_vec<std::add_pointer_t<void(convar*, const char*, float)>> on_change_callbacks{ };
};

}
