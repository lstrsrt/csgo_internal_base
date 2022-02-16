#include "netvars.h"
#include "../memory/interface.h"
#include "../crypt/fnv1a.h"

void netvars::initialize() noexcept
{ 
    for (auto list = interfaces::client->get_all_classes(); list; list = list->next) {
        if (!list->recv_table)
            continue;

        dump_table(list->network_name, list->recv_table, 0);
    }
    LOG_INFO("Netvars initialized.");
}

void netvars::dump_table(std::string_view base_class, cs::recv_table* table, const uint32_t offset) noexcept
{
    for (int i = 0; i < table->prop_amt; i++) {
        const auto prop = &table->props[i];

        // Skip useless classes.
        if (isdigit(prop->var_name[0]) || fnv1a::rt(prop->var_name) == fnv1a::ct("baseclass"))
            continue;

        // Follow to the next datatable, provided it has props.
        if (prop->data_table && prop->recv_type == cs::send_prop_type::dpt_datatable 
            && prop->data_table->net_table_name[0] == 'D' && prop->data_table->prop_amt > 0)
            dump_table(base_class, prop->data_table, offset + prop->offset);

        const auto name = std::string(base_class.data()) + "->" + prop->var_name;
        const auto hash = fnv1a::rt(name);

        static std::fstream file{ "netvars.txt", std::fstream::out | std::fstream::trunc };
        if (file.good())
            file << name << '\n';

        var_map[hash] = std::make_pair(static_cast<uint32_t>(offset + prop->offset), prop);
    }
}

void netvars::set_proxy(hash_t name, cs::recv_proxy_fn proxy, cs::recv_proxy_fn& original) noexcept
{
    auto prop = var_map[name].second;
    original = prop->proxy_fn;
    prop->proxy_fn = proxy;
    LOG_SUCCESS("Set proxy for prop {}.", prop->var_name);
}

uint32_t netvars::get_datamap_offset(cs::datamap* map, const hash_t field_name) noexcept
{
    while (map) {
        for (int i = 0; i < map->data_fields_count; i++) {
            if (!map->data_description[i].field_name)
                continue;

            if (field_name == fnv1a::rt(map->data_description[i].field_name))
                return map->data_description[i].field_offset;

            // If we didn't find it, search recursively.
            if (map->data_description[i].type != cs::field_type::embedded || !map->data_description[i].data_map)
                continue;

            const auto offset = get_datamap_offset(map->data_description[i].data_map, field_name);
            if (!offset)
                continue;

            return offset;
        }
        map = map->base_map;
    }
    return 0;
}
