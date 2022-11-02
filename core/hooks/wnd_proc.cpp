#include "../hooks.h"
#include "../cheat.h"
#include "../../render/menu/menu.h"

LRESULT CALLBACK hooks::wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle }))
        menu::toggle();

    if (input::is_key_active({ VK_DELETE, input::key_type::toggle }))
        cheat::should_unhook = true;

    interfaces::input_system->enable_input(!menu::is_open);

    if (menu::is_open)
        return TRUE;

    return CallWindowProcA(original_wnd_proc, hwnd, msg, wparam, lparam);
}
