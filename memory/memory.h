#pragma once

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(size) private: [[maybe_unused]] std::byte CONCAT(pad, __COUNTER__)[size]{ }; public:

#include <intrin.h>

#include "address.h"
#include "dll.h"

namespace memory {

    struct virtual_alloc {
        LPVOID address{ };
        bool free{ };

        explicit virtual_alloc(SIZE_T size, DWORD flags) noexcept
        {
            address = VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, flags);
            if (address)
                free = true;
        }

        ~virtual_alloc()
        {
            if (free)
                VirtualFree(address, 0, MEM_RELEASE);
        }
    };

    struct virtual_protect {
        LPVOID address{ };
        SIZE_T size{ };
        DWORD flags{ };

        explicit virtual_protect(LPVOID addr, SIZE_T size, DWORD flags) noexcept
            : address(addr), size(size)
        {
            VirtualProtect(address, size, flags, &flags);
        }

        ~virtual_protect()
        {
            VirtualProtect(address, size, flags, &flags);
        }
    };

    template<class ty, int i, class... va_args>
    ty call_virtual(void* base, va_args... args) noexcept
    {
        return (*static_cast<ty(__thiscall***)(void*, va_args...)>(base))[i](base, args...);
    }

    template<class ty = address>
    ty get_virtual(void* base, int index) noexcept
    {
        return (*static_cast<ty**>(base))[index];
    }

    inline address get_frame_address() noexcept
    {
        return address(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof(uintptr_t));
    }

    inline bool is_address_valid(address addr) noexcept
    {
        if (!addr)
            return false;

        MEMORY_BASIC_INFORMATION info{ };
        if (!VirtualQuery(addr, &info, sizeof(info)))
            return false;

        return info.State & MEM_COMMIT && !(info.Protect & PAGE_NOACCESS);
    }

    inline size_t get_vmt_length(uintptr_t* vptr) noexcept
    {
        size_t length{ };
        MEMORY_BASIC_INFORMATION info{ };

        while (VirtualQuery(reinterpret_cast<LPCVOID>(vptr[length]), &info, sizeof(info)) &&
               info.State & MEM_COMMIT &&
               info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
            ++length;

        return length;
    }

    template<class ty>
    bool read(address addr, ty& data, SIZE_T size = sizeof(ty)) noexcept
    {
        return ReadProcessMemory(HANDLE(-1), addr, &data, size, nullptr);
    }

    template<class ty>
    bool write(address addr, ty& data, SIZE_T size = sizeof(ty)) noexcept
    {
        return WriteProcessMemory(HANDLE(-1), addr, &data, size, nullptr);
    }

    inline bool protect(address addr, DWORD flags, SIZE_T size) noexcept
    {
        DWORD _;
        return VirtualProtect(addr, size, flags, &_);
    }

    inline bool query(address addr, MEMORY_BASIC_INFORMATION& mem_info) noexcept
    {
        return VirtualQuery(addr, &mem_info, sizeof(mem_info));
    }

}

#define VIRTUAL_FUNCTION(name, ret, idx, params, args) \
inline ret name params noexcept \
{ \
    return memory::call_virtual<ret, idx>args; \
}

// Parameters and arguments are reversed, only way I could get this macro to work properly.
#define VIRTUAL_FUNCTION_SIG(name, ret, dll, sig, args, ... /* params */) \
inline ret name(__VA_ARGS__) noexcept \
{ \
    static ret(__thiscall* fn)(void*, __VA_ARGS__) = dll.find(PATTERN(sig)).cast<decltype(fn)>(); \
    return fn args; \
}
