#include "memory.h"

address memory::find_bytes(dll dll_id, std::string_view pattern) noexcept
{
    constexpr int wildcard = -1;

    std::istringstream stream{ pattern.data() };
    std::string buf{ };
    std::vector<int> byte_pattern{ };
    
    while (stream >> buf) {
        if (buf.at(0) == '?')
            byte_pattern.push_back(wildcard);
        else if (isxdigit(buf.at(0)) && isxdigit(buf.at(1)))
            byte_pattern.push_back(stoi(buf, 0, 16));
    }
    
    auto& dll = dlls::get(dll_id);
    auto start = reinterpret_cast<uint8_t*>(dll.nt_headers->OptionalHeader.ImageBase);
    const auto pattern_size = byte_pattern.size();
    const auto pattern_data = byte_pattern.data();

    for (size_t i = 0; i < dll.nt_headers->OptionalHeader.SizeOfImage - pattern_size; i++) {
        bool found = true;
        for (size_t j = 0; j < pattern_size; j++) {
            if (start[i + j] != pattern_data[j] && pattern_data[j] != wildcard) {
                found = false;
                break;
            }
        }
        if (found)
            return address(&start[i]);
    }

    LOG_ERROR("Pattern {} was not found in {}!", pattern, dll.name);
    return address();
}

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
