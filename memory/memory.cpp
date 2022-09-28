#include "memory.h"

size_t memory::get_vmt_length(uintptr_t* vptr) noexcept
{
    size_t length{ };
    MEMORY_BASIC_INFORMATION info{ };

    while (VirtualQuery(reinterpret_cast<LPCVOID>(vptr[length]), &info, sizeof(info)) &&
        info.State & MEM_COMMIT &&
        info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
            ++length;

    return length;
}
