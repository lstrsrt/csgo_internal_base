#pragma once

namespace cs {

struct key_values {
    VIRTUAL_FUNCTION_SIG(init, void, dll::client, "55 8B EC 51 33 C0 C7 45", (this, name), const char* name)

    void load_from_buffer(const char* resource_name, const char* buffer /* Omitted unnecessary arguments */) noexcept
    {
        static void(__thiscall* load_from_buffer_fn)(key_values*, const char*, const char*, void*, const char*, void*, void*) = 
            memory::find_bytes(dll::client, "55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89").cast<decltype(load_from_buffer_fn)>();
        return load_from_buffer_fn(this, resource_name, buffer, nullptr, nullptr, nullptr, nullptr);
    }

    VIRTUAL_FUNCTION_SIG(find_key, key_values*, dll::client, "55 8B EC 83 EC 1C 53 8B D9 85 DB", (this, key_name, create_new), 
        const char* key_name, bool create_new)

    void set_string(const char* key_name, const char* value) noexcept
    {
        static void(__thiscall* set_string_fn)(key_values*, void*, const char*) = 
            memory::find_bytes(dll::client, "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01").cast<decltype(set_string_fn)>();

        const auto key = find_key(key_name, false);
        if (!key)
            return;

        set_string_fn(this, key, value);
    }

    PAD(0x24)
};

}
