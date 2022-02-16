#pragma once

namespace se {

struct panel {
    VIRTUAL_FUNCTION(get_panel_name, const char*, 36, (cs::vgui_panel panel_id), (this, panel_id))
};

}
