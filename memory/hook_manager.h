#pragma once

#include <unordered_map>

#include "memory.h"

#define SET_SIG_HOOK(dll, sig, name) set(dll, PATTERN(sig), reinterpret_cast<void*>(name::fn), reinterpret_cast<void**>(&name::original))
#define SET_VT_HOOK(vmt, name, index) set(vmt, index, name::fn, name::original)
#define SET_IAT_HOOK(dll, name, target) iat::set(dlls::get(dll), iat::hook::name, target, reinterpret_cast<uintptr_t>(iat::name::fn))

namespace hooks {

    inline static std::add_pointer_t<bool(void*, void*, void*, bool)> hook_func{ };
    inline static std::add_pointer_t<void(void*, bool)> unhook_func;

    inline std::unordered_map<void*, void*> hooked_fns{ };

    void initialize() noexcept;
    void end() noexcept;

    template<class ty, class fn>
    inline void set(interface_holder<ty*>& vmt, int index, void* hook, fn& original) noexcept
    {
        vmt.replacement_vmt[index + 1] = reinterpret_cast<uintptr_t>(hook);
        original = reinterpret_cast<fn>(vmt.real_vmt[index]);
    }

    inline void set(void* base, int index, void* hook, void** original) noexcept
    {
        const auto target = memory::get_virtual(base, index).cast<void*>();

        if (hook_func(target, hook, original, false))
            return;

        LOG_ERROR("Error while hooking function!");
    }

    template<size_t len>
    inline void set(dll dll_id, std::array<int, len>&& sig, void* hook, void** original) noexcept
    {
        const auto target = memory::find_bytes<len>(dll_id, std::move(sig)).cast<void*>();

        if (hook_func(target, hook, original, false))
            hooked_fns[hook] = original;
        else
            LOG_ERROR("Error while hooking function!");
    }

    inline void unset(void* hook) noexcept
    {
        unhook_func(hooked_fns[hook], false);
    }

}
