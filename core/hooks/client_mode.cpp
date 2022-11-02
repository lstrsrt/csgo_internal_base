#include "../hooks.h"
#include "../features/features.h"

void __fastcall hooks::override_view::fn(se::client_mode* ecx, int, cs::view_setup* view)
{
    if (!local || !local.in_game)
        return original(ecx, view);

    if (!local->is_scoping())
        view->fov = cfg::get<float>(vars::fov);

    return original(ecx, view);
}

float __fastcall hooks::get_viewmodel_fov::fn(se::client_mode* ecx, int)
{
    if (!local || local->is_scoping())
        return original(ecx);

    return cfg::get<float>(vars::viewmodel_fov);
}
