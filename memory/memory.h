#pragma once

#include <intrin.h>

#include "address.h"
#include "dll.h"

namespace memory {

    template<class ty, int i, typename... va_args>
    inline ty call_virtual(void* base, va_args... args) noexcept
    {
        return (*static_cast<ty(__thiscall***)(void*, va_args...)>(base))[i](base, args...);
    }
    
    template<class ty = address>
    inline ty get_virtual(void* base, int index) noexcept
    {
        return (*static_cast<ty**>(base))[index];
    }
    
    inline address get_frame_address() noexcept
    {
        return address(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof(uintptr_t));
    }

    address find_bytes(dll dll_id, std::string_view pattern) noexcept;
    size_t get_vmt_length(uintptr_t* vptr) noexcept;

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
    static ret(__thiscall* name##_fn)(void*, __VA_ARGS__) = memory::find_bytes(dll, sig).cast<decltype(name##_fn)>(); \
    return name##_fn##args; \
}
