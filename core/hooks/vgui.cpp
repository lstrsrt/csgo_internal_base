#include "../hooks.h"
#include "../features/features.h"
#include "../../render/menu/menu.h"

void __fastcall hooks::paint::fn(se::vgui* ecx, int, cs::paint_mode mode)
{
    static bool once = [] { return render::initialize(); }();

    if (ecx->static_transition_panel && (mode & cs::paint_mode::paint_ui_panels)) {
        interfaces::surface->start_drawing();

        visuals::esp::run();
        menu::draw_watermark();
        menu::run();

        interfaces::surface->finish_drawing();
    }

    return original(ecx, mode);
}
