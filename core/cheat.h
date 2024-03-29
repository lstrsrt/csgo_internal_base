#pragma once

#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"
#include "../valve/cs/player.h"
#include "netvars.h"
#include "config.h"

namespace cheat {

    inline cs::local_player local{ };
    inline bool should_unhook{ };

    inline void initialize() noexcept
    {
        logger::initialize(L"csgo", L"log.txt");
        dlls::initialize();
        interfaces::initialize();
        netvars::initialize();
        hooks::initialize();
        cfg::initialize();

        LOG_SUCCESS("Cheat initialized. Last full build: {} {}", __DATE__, __TIME__);
    }

    inline DWORD end(LPVOID instance) noexcept
    {
        interfaces::input_system->enable_input();
        hooks::end();
        logger::end();

        FreeLibraryAndExitThread(static_cast<HMODULE>(instance), EXIT_SUCCESS);
        return EXIT_SUCCESS;
    }

}
