#pragma once

#include "dll.h"
#include "../base/logger.h"

static IMAGE_IMPORT_DESCRIPTOR* get_image_import_descriptor(void* base) noexcept
{
    const auto nt_headers = reinterpret_cast<IMAGE_NT_HEADERS*>(reinterpret_cast<CHAR*>(base) + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
    if (nt_headers->Signature != IMAGE_NT_SIGNATURE || nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
        return nullptr;

    const auto dir_addr = nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    return reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(reinterpret_cast<ULONG_PTR>(base) + dir_addr);
}

// Probably not needed (I just pasted this in from another project), delete this if you want

namespace hooks {
    namespace iat {
    
    enum class hook {
        // ...
    };
    
    inline std::unordered_map<hook, void**> original_functions;
    
    template<class ret = void, typename... va_args>
    ret call_original(hook hk, va_args... args) noexcept
    {
        return reinterpret_cast<ret(__stdcall*)(va_args...)>(original_functions.find(hk)->second)(args...);
    }
    
    void set(const dll_t& dll, hook hk, std::string_view name, const uintptr_t hook) noexcept
    {
        original_functions.emplace(hk, reinterpret_cast<void**>(GetProcAddress(dll.hmod, name.data())));
        
        auto base_address = GetModuleHandleA(nullptr);
        auto import_desc = get_image_import_descriptor(base_address);
        if (!import_desc) {
            LOG_ERROR("Incorrect import descriptor for {}!", dll.name);
            return;
        }
        
        while (import_desc->Name) {
            auto cur_dll = reinterpret_cast<char*>(base_address) + import_desc->Name;
            if (!_stricmp(dll.name.c_str(), cur_dll)) {
                auto thunk_data = reinterpret_cast<IMAGE_THUNK_DATA*>(reinterpret_cast<DWORD>(base_address) + import_desc->OriginalFirstThunk);
                for (int i = 0; thunk_data->u1.Function; i++, thunk_data++) {
                    char* cur_fn = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(reinterpret_cast<DWORD>(base_address) + thunk_data->u1.AddressOfData)->Name;
                    if (!_stricmp(name.data(), cur_fn)) {
                        auto table = reinterpret_cast<uintptr_t*>(base_address + import_desc->FirstThunk);
                        winapi::virtual_protect vp{ &table[i], PAGE_READWRITE };
                        table[i] = hook;
                        LOG_SUCCESS("Hooked {} in {}.", name, dll.name);
                        return;
                    }
                }
                LOG_ERROR("Did not find {} in {}!", name, dll.name);
                return;
            }
            import_desc++;
        }
        LOG_ERROR("{} is not in the import descriptor!", dll.name);
    }

    }
}
