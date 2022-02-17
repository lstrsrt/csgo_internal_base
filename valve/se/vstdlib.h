#pragma once

#include "../cs/convar.h"

namespace se {

struct cvar {
    VIRTUAL_FUNCTION(find_cmd_line_value, const char*, 11, (const char* name), (this, name))
    VIRTUAL_FUNCTION(find_var, cs::convar*, 14, (const char* var_name), (this, var_name))

    class iterator
    {
    public:
        VIRTUAL_FUNCTION(set_first, void, 0, (), (this))
        VIRTUAL_FUNCTION(next, void, 1, (), (this))
        VIRTUAL_FUNCTION(is_valid, bool, 2, (), (this))
        VIRTUAL_FUNCTION(get, cs::command_base*, 3, (), (this))
    };

    VIRTUAL_FUNCTION(factory_internal_iterator, iterator*, 42, (), (this))

    cs::utl_vec<std::add_pointer_t<void(cs::convar*, const char*, float)>> global_change_callbacks{ };
    PAD(0x1c)
    cs::convar* cmd_list{ };
};

struct key_values_system;

using key_values_system_fn = key_values_system*(*)();

}
