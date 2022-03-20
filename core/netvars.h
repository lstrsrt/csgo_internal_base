#pragma once

#include <unordered_map>

#include "../base/base.h"
#include "../crypt/fnv1a.h"
#include "../valve/cs/datatables.h"

#define SET_PROXY(name, fn) netvars::set_proxy(fnv1a::ct(name), fn::proxy, fn::original)

namespace netvars {

    inline std::unordered_map<hash_t, std::pair<uint32_t, cs::recv_prop*>> var_map{ };
    
    void initialize() noexcept;
    void dump_table(std::string_view base_class, cs::recv_table* table, const uint32_t offset) noexcept;
    void set_proxy(hash_t name, cs::recv_proxy_fn proxy, cs::recv_proxy_fn& original) noexcept;
    uint32_t get_datamap_offset(cs::datamap* map, const hash_t field_name) noexcept;

    inline hash_t get(hash_t hash) noexcept
    {
        return var_map.at(hash).first;
    };

}

#define NETVAR(name, type, var_name) NETVAR_OFFSET(name, type, var_name, 0)

#define NETVAR_OFFSET(name, type, var_name, offset) \
inline type& name() { \
    constexpr auto hash = fnv1a::ct(var_name); \
    return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + netvars::get(hash) + offset); \
}

#define PTR_NETVAR(name, type, var_name) PTR_NETVAR_OFFSET(name, type, var_name, 0)

#define PTR_NETVAR_OFFSET(name, type, var_name, offset) \
inline std::add_pointer_t<type> name() { \
    constexpr auto hash = fnv1a::ct(var_name); \
    return std::add_pointer_t<type>(reinterpret_cast<uintptr_t>(this) + netvars::get(hash) + offset); \
}

#define ARRAY_NETVAR(name, type, size, var_name) \
inline auto& name() { \
    constexpr auto hash = fnv1a::ct(var_name); \
    return *reinterpret_cast<std::add_pointer_t<std::array<type, size>>>(reinterpret_cast<uintptr_t>(this) + netvars::get(hash)); \
}

#define DATAMAP_FIELD(name, type, map, var_name) \
inline auto name() { \
    constexpr auto hash = fnv1a::ct(var_name); \
    static const auto offset = netvars::get_datamap_offset(map, hash); \
    return *reinterpret_cast<std::add_pointer_t<type>>(reinterpret_cast<uintptr_t>(this) + offset); \
}

#define PTR_DATAMAP_FIELD(name, type, map, var_name) \
inline auto name() { \
    constexpr auto hash = fnv1a::ct(var_name); \
    static const auto offset = netvars::get_datamap_offset(map, hash); \
    return reinterpret_cast<std::add_pointer_t<type>>(reinterpret_cast<uintptr_t>(this) + offset); \
}

#define OFFSET(name, type, diff) \
inline auto& name() { \
    return *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + diff); \
}

#define PTR_OFFSET(name, type, diff) \
inline auto* name() { \
    return reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(this) + diff); \
}
