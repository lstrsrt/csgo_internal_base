#pragma once

#include <map>

#include "memory.h"

#define SET_SIG_HOOK(dll, sig, name) set(dll, PATTERN(sig), reinterpret_cast<void*>(name::fn), reinterpret_cast<void**>(&name::original))
#define SET_VT_HOOK(vmt, name, index) set(vmt, index, name::fn, name::original)

namespace hooks {

    inline static std::add_pointer_t<bool(void*, void*, void*, bool)> hook_func{ };
    inline static std::add_pointer_t<void(void*, bool)> unhook_func;

    inline std::map<void*, void*> hooked_fns{ };

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
    inline void set(dll& dll, std::array<int, len>&& sig, void* hook, void** original) noexcept
    {
        const auto target = dll.find<len>(std::move(sig)).cast<void*>();

        hooked_fns[hook] = target;

        if (hook_func(target, hook, original, false))
            return;

        LOG_ERROR("Error while hooking function!");
    }

}
