#pragma once

#include <unordered_map>
#include <Windows.h>
#include <winternl.h>

#include "../base/base.h"
#include "../crypt/fnv1a.h"
#include "address.h"

struct dll;
namespace dlls { inline std::vector<dll*> list; }

struct dll {
    std::string name{ };
    uintptr_t base{ };
    size_t size{ };
    address create_interface{ }; // Only relevant to game DLLs

    explicit dll(const char* name) noexcept
        : name(name)
    {
        dlls::list.push_back(this);
    }

    template<size_t len> requires(len > 0)
    address find(std::array<int, len>&& pattern) const noexcept
    {
        static int i{ };
        i++;

        auto bytes = reinterpret_cast<uint8_t*>(base);
        for (size_t i{ }; i < size - len; i++) {
            for (size_t j{ }; j < len; j++) {
                if (bytes[i + j] != pattern[j] && pattern[j] != -1)
                    break;
                if (j + 1 == len)
                    return address(&bytes[i]);
            }
        }

        LOG_ERROR("Did not find pattern {} in module {}!", i, name);
        return address();
    }

    inline address get_export(hash_t hash) const noexcept
    {
        const auto nt_optional_hdr = [this]() -> const IMAGE_OPTIONAL_HEADER32* {
            auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
            if (dos->e_magic != IMAGE_DOS_SIGNATURE)
                return nullptr;

            auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
            if (nt->Signature != IMAGE_NT_SIGNATURE)
                return nullptr;

            return &nt->OptionalHeader;
        }();

        if (!nt_optional_hdr)
            return address();

        const auto dir_addr = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(
            base + nt_optional_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        const auto functions = reinterpret_cast<uint32_t*>(base + dir_addr->AddressOfFunctions);
        const auto names = reinterpret_cast<uint32_t*>(base + dir_addr->AddressOfNames);
        const auto ordinals = reinterpret_cast<uint16_t*>(base + dir_addr->AddressOfNameOrdinals);

        for (DWORD i{ }; i < dir_addr->NumberOfFunctions; i++) {
            const auto name = reinterpret_cast<const char*>(base + names[i]);
            if (fnv1a::hash(name) == hash)
                return address(base + functions[ordinals[i]]);
        }

        return address();
    }
};

namespace dlls {

    inline dll client{ "client.dll" };
    inline dll datacache{ "datacache.dll" };
    inline dll engine{ "engine.dll" };
    inline dll file_system{ "filesystem_stdio.dll" };
    inline dll game_overlay_renderer{ "gameoverlayrenderer.dll" };
    inline dll input_system{ "inputsystem.dll" };
    inline dll localize{ "localize.dll" };
    inline dll material_system{ "materialsystem.dll" };
    inline dll matchmaking{ "matchmaking.dll" };
    inline dll server{ "server.dll" };
    inline dll shader_api_dx9{ "shaderapidx9.dll" };
    inline dll steam_api{ "steam_api.dll" };
    inline dll studio_render{ "studiorender.dll" };
    inline dll tier0{ "tier0.dll" };
    inline dll vgui_mat_surface{ "vguimatsurface.dll" };
    inline dll vgui2{ "vgui2.dll" };
    inline dll vphysics{ "vphysics.dll" };
    inline dll vstdlib{ "vstdlib.dll" };

    inline void initialize() noexcept
    {
        struct ldr_data_table_entry {
            LIST_ENTRY InMemoryOrderLinks;
            PAD(0x8);
            PVOID DllBase;
            PAD(0x4);
            ULONG SizeOfImage;
            PAD(0x8);
            UNICODE_STRING BaseDllName;
            PAD(0x7c);
        };

        std::unordered_map<hash_t, ldr_data_table_entry*> loaded{ };

        const auto peb = reinterpret_cast<const PEB*>(__readfsdword(0x30));
        auto cur = CONTAINING_RECORD(peb->Ldr->InMemoryOrderModuleList.Flink,
                                     ldr_data_table_entry,
                                     InMemoryOrderLinks);

        while (cur->BaseDllName.Length) {
            loaded.insert_or_assign(fnv1a::hash(cur->BaseDllName.Buffer), cur);
            LOG_INFO(L"Found module {} at {}", cur->BaseDllName.Buffer, cur->DllBase);
            cur = reinterpret_cast<ldr_data_table_entry*>(cur->InMemoryOrderLinks.Flink);
        }

        for (auto entry : list) {
            const auto res = loaded.find(fnv1a::hash(entry->name));
            ASSERT_MSG(res != loaded.cend(), "DLL not loaded yet?");
            const auto dll = res->second;
            entry->base = reinterpret_cast<uintptr_t>(dll->DllBase);
            entry->size = dll->SizeOfImage;
        }
    }

};
