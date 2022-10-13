#include "entity.h"
#include "../../memory/interfaces.h"

namespace cs {

entity_type base_entity::get_entity_type() noexcept
{
    switch (get_client_class()->id) {
    case class_id::cs_player:
        return entity_type::player;
    case class_id::base_cs_grenade_projectile:
    case class_id::breach_charge_projectile:
    case class_id::decoy_projectile:
    case class_id::molotov_projectile:
    case class_id::sensor_grenade_projectile:
    case class_id::smoke_grenade_projectile:
    case class_id::base_cs_grenade:
    case class_id::breach_charge:
    case class_id::decoy_grenade:
    case class_id::flashbang:
    case class_id::he_grenade:
    case class_id::incendiary_grenade:
    case class_id::molotov_grenade:
    case class_id::sensor_grenade:
    case class_id::smoke_grenade:
    case class_id::particle_smoke_grenade:
    case class_id::inferno:
        return entity_type::grenade;
    case class_id::c4:
    case class_id::planted_c4:
        return entity_type::bomb;
    case class_id::weapon_ak47:
    case class_id::deagle:
    case class_id::weapon_aug:
    case class_id::weapon_awp:
    case class_id::weapon_base_item:
    case class_id::weapon_ppbizon:
    case class_id::weapon_cs_base:
    case class_id::weapon_cs_base_gun:
    case class_id::weapon_cycler:
    case class_id::weapon_dual_berettas:
    case class_id::weapon_famas:
    case class_id::weapon_fiveseven:
    case class_id::weapon_g3sg1:
    case class_id::weapon_galil:
    case class_id::weapon_galil_ar:
    case class_id::weapon_glock18:
    case class_id::weapon_p2000:
    case class_id::weapon_m249:
    case class_id::weapon_m3:
    case class_id::weapon_m4a4:
    case class_id::weapon_mac10:
    case class_id::weapon_mag7:
    case class_id::weapon_mp5sd:
    case class_id::weapon_mp7:
    case class_id::weapon_mp9:
    case class_id::weapon_negev:
    case class_id::weapon_nova:
    case class_id::weapon_p228:
    case class_id::weapon_p250:
    case class_id::weapon_p90:
    case class_id::weapon_sawedoff:
    case class_id::weapon_scar20:
    case class_id::weapon_scout:
    case class_id::weapon_sg550:
    case class_id::weapon_sg552:
    case class_id::weapon_sg553:
    case class_id::weapon_ballistic_shield:
    case class_id::weapon_ssg08:
    case class_id::weapon_zeusx27:
    case class_id::weapon_tec9:
    case class_id::weapon_tmp:
    case class_id::weapon_ump45:
    case class_id::weapon_usp:
    case class_id::weapon_xm1014:
        return entity_type::weapon;
    default:
        return entity_type::other;
    }

}

weapon_info* weapon::get_info() noexcept
{
    return interfaces::weapon_system->get_weapon_data(get_item_id());
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
