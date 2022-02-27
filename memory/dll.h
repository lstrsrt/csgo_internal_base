#pragma once

#include <Windows.h>
#include <Psapi.h>

#include "../base/base.h"

struct dll_t {
    std::string name{ };
    HMODULE hmod{ };
    IMAGE_NT_HEADERS32* nt_headers{ };
    uintptr_t create_interface{ }; // Only relevant to game DLLs

    dll_t(const char* name) noexcept
        : name(name) {};
};

enum class dll {
    client,
    datacache,
    engine,
    filesystem,
    game_overlay_renderer,
    input_system,
    localize,
    material_system,
    matchmaking,
    server,
    shader_api_dx9,
    steam_api,
    studio_render,
    tier0,
    vgui_mat_surface,
    vgui2,
    vphysics,
    vstdlib
};

namespace dlls {

    // Must be in the same order as the enum!
    inline std::vector<dll_t> list{ "client.dll", "datacache.dll", "engine.dll", "filesystem_stdio.dll", "gameoverlayrenderer.dll", 
    "inputsystem.dll", "localize.dll", "materialsystem.dll", "matchmaking.dll", "server.dll", "shaderapidx9.dll", "steam_api.dll", 
    "studiorender.dll", "tier0.dll", "vguimatsurface.dll", "vgui2.dll", "vphysics.dll", "vstdlib.dll" };

    inline void initialize() noexcept
    {
        while (!GetModuleHandleA("serverbrowser.dll"))
            std::this_thread::sleep_for(100ms);

        for (auto& dll : list) {
            if (!(dll.hmod = GetModuleHandleA(dll.name.c_str())))
                LOG_ERROR("{} does not have a valid module handle!", dll.name);
            else {
                dll.nt_headers = reinterpret_cast<IMAGE_NT_HEADERS32*>(reinterpret_cast<char*>(dll.hmod) +
                    reinterpret_cast<IMAGE_DOS_HEADER*>(dll.hmod)->e_lfanew);

                if (dll.nt_headers->Signature == IMAGE_NT_SIGNATURE)
                    LOG_INFO("Retrieved module information for {}.", dll.name);
                else
                    LOG_ERROR("Invalid NT header for {}!", dll.name);
            }
        }
    }

    inline dll_t& get(dll id) noexcept
    {
        return list.at(static_cast<size_t>(id));
    }

};
