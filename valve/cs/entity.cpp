#include "entity.h"
#include "../../memory/interface.h"

namespace cs {

weapon_info* weapon::get_info() noexcept
{
    return interfaces::weapon_system->get_weapon_data(this->get_item_id());
}

float planted_c4::get_remaining_time() noexcept
{
    return std::clamp(get_blow_time() - interfaces::globals->cur_time, 0.0f, get_total_time());
}

float planted_c4::get_remaining_defuse_time() noexcept
{
    return std::clamp(get_defuse_countdown() - interfaces::globals->cur_time, 0.0f, get_defuse_length());
}

}
