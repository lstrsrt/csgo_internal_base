#include "disasm/disasm.hh"
#include "detour.h"
#include "memory.h"

static constexpr uint8_t jmp_rel = 0xe9;
static constexpr size_t jmp_size = 5;

static bool is_page_rwx(void* addr) noexcept
{
    MEMORY_BASIC_INFORMATION mem_info{ };
    return (VirtualQuery(addr, &mem_info, sizeof(mem_info)) &&
            mem_info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY));
}

static void set_jmp(void* dst, void* src) noexcept
{
    auto ptr = static_cast<uint8_t*>(src);
    *ptr = jmp_rel;
    *reinterpret_cast<uintptr_t*>(ptr + 1) = reinterpret_cast<uintptr_t>(dst) - reinterpret_cast<uintptr_t>(ptr) - jmp_size;
}

bool detour::hook::set(bool enable) noexcept
{
    if (!memory::is_address_valid(target) || !memory::is_address_valid(hook_fn))
        return false;

    if (!is_page_rwx(target))
        return false;

    static constexpr size_t max_trampoline_size = 40;
    memory::virtual_alloc trampoline_mem{ max_trampoline_size, PAGE_EXECUTE_READWRITE };
    trampoline = trampoline_mem.address;
    if (!trampoline)
        return false;

    auto target = static_cast<uint8_t*>(this->target);
    disasm::u32 size{ };
    while (size < jmp_size) {
        auto insn = disasm::disasm(target + size);
        size += insn.length;
        if (insn.error() || !insn.length || size >= max_trampoline_size)
            return false;
    }
    std::copy(target, target + size, static_cast<uint8_t*>(trampoline));
    std::copy(target, target + jmp_size, real_bytes.data());
    set_jmp(target + size, static_cast<uint8_t*>(trampoline) + size);

    trampoline_mem.free = false;
    *original = trampoline;

    if (enable)
        this->enable();

    return true;
}

void detour::hook::enable() noexcept
{
    memory::virtual_protect vp{ target, jmp_size, PAGE_EXECUTE_READWRITE };
    set_jmp(hook_fn, target);
}

void detour::hook::disable() noexcept
{
    memory::virtual_protect vp{ target, jmp_size, PAGE_EXECUTE_READWRITE };
    std::ranges::copy(real_bytes, static_cast<uint8_t*>(target));
}

void detour::hook::remove() noexcept
{
    disable();
    VirtualFree(trampoline, 0, MEM_RELEASE);
}
