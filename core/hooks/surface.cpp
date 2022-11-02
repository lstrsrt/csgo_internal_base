#include "../hooks.h"
#include "../../render/menu/menu.h"

void __fastcall hooks::lock_cursor::fn(se::surface* ecx, int)
{
    return menu::is_open ? ecx->unlock_cursor() : original(ecx);
}
