#pragma once

namespace cs {

struct surface_data {
    PAD(0x58)
    float penetration_modifier{ };
    float damage_modifier{ };
    mat_handle material{ };
};

}

namespace se {

struct physics_surface_props {
    VIRTUAL_FUNCTION(get_surface_data, cs::surface_data*, 5, (int index), (this, index))
};

}
