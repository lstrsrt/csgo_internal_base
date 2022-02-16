#pragma once

#include "../memory/interface.h"
#include "../memory/hook_manager.h"
#include "netvars.h"
#include "config.h"

namespace cs { struct local_player; }

namespace cheat {
    
    inline cs::local_player* local_player{ };
    inline cs::user_cmd* cmd{ };
    inline angle view{ };
    inline bool should_detach{ };

    inline void initialize() noexcept
    {
        logger::initialize(L"csgo", L"log.txt");
        dlls::initialize();
        interfaces::initialize();
        netvars::initialize();
        hooks::initialize();
        config::initialize();

        LOG_SUCCESS("Cheat initialized. Last full build: {} {}", __DATE__, __TIME__);
    }
    
    inline void end() noexcept
    {
        interfaces::input_system->enable_input();
        hooks::end();
        logger::end();
    }
    
}
