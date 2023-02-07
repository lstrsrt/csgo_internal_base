#include "../hooks.h"
#include "../cheat.h"
#include "../features/variables.h"
#include "../../render/menu/menu.h"

LRESULT CALLBACK hooks::wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle }))
        menu::toggle();

    if (input::is_key_active(cfg::get<keybind>(vars::panic_key)))
        cheat::should_unhook = true;

    interfaces::input_system->enable_input(!menu::is_open);

    return menu::is_open ? TRUE : CallWindowProcA(original_wnd_proc, wnd, msg, wparam, lparam);
}
