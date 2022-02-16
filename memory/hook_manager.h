#pragma once

#include <unordered_map>

#include "memory.h"

#define SET_SIG_HOOK(dll, sig, name) set(dll, sig, reinterpret_cast<void*>(name::fn), reinterpret_cast<void**>(&name::original), #name)
#define SET_DETOUR_HOOK(base, name) set(base, name::index, reinterpret_cast<void*>(name::fn), reinterpret_cast<void**>(&name::original), #name)
#define SET_VF_HOOK(vmt, name) set(vmt, name::index, name::fn, name::original, #name)
#define SET_IAT_HOOK(dll, name, target) iat::set(g_dlls(dll), iat::hook::name, target, reinterpret_cast<uintptr_t>(iat::name::fn))

namespace hooks {

    inline static std::add_pointer_t<bool(void*, void*, void*, bool)> hook_func{ };
    inline static std::add_pointer_t<void(void*, bool)> unhook_func;

    inline std::unordered_map<hash_t, void*> hooked_fns{ };

    void initialize() noexcept;
    void end() noexcept;

    template<class ty, class fn>
    inline void set(interface_holder<ty*>& vmt, int index, void* hook, fn& original, std::string_view name) noexcept
    {
        vmt.replacement_vmt[index + 1] = reinterpret_cast<uintptr_t>(hook);
        original = reinterpret_cast<fn>(vmt.real_vmt[index]);
        LOG_SUCCESS("Hooked {}.", name);
    }
    
    inline void set(void* base, int index, void* hook, void** original, std::string_view name) noexcept
    {
        const auto target = memory::get_virtual(base, index).cast<void*>();

        if (hook_func(target, hook, original, false))
            LOG_SUCCESS("Detoured {}.", name);
        else
            LOG_ERROR("Unable to hook {}!", name);
    }

    inline void set(dll dll_id, std::string_view sig, void* hook, void** original, std::string_view name) noexcept
    {
        const auto target = memory::find_bytes(dll_id, sig).cast<void*>();
        hooked_fns[fnv1a::rt(name.data())] = target;

        if (hook_func(target, hook, original, false))
            LOG_SUCCESS("Detoured {} (signature: {}).", name, sig);
        else
            LOG_ERROR("Unable to hook {}!", name);
    }

}
