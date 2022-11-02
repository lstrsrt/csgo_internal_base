#include "../hooks.h"
#include "../features/features.h"
#include "../features/cache.h"

void __fastcall hooks::on_add_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle)
{
    auto unknown = reinterpret_cast<cs::unknown*>(handle_entity);
    auto base_entity = unknown->get_base_entity();
    if (base_entity)
        cache::add(base_entity);

    return original(ecx, handle_entity, handle);
}

void __fastcall hooks::on_remove_entity::fn(se::entity_list* ecx, int, cs::handle_entity* handle_entity, cs::base_handle handle)
{
    auto unknown = reinterpret_cast<cs::unknown*>(handle_entity);
    auto base_entity = unknown->get_base_entity();
    if (base_entity)
        cache::remove(base_entity);

    return original(ecx, handle_entity, handle);
}
