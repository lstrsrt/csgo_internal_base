#pragma once

#include "../../core/netvars.h"
#include "../../memory/memory.h"

#include "entity.h"

namespace cs {

constexpr int max_players = 64;
constexpr int multiplayer_backup = 150;
constexpr int tick_never_think = -1;

struct studio_hdr;

enum class life_state {
    alive,
    dead = 2
};

enum class move_state {
    idle,
    walk,
    run
};

enum class user_message {
    vote_start = 46,
    vote_pass = 47,
    vote_failed = 48,
    server_rank_reveal_all = 50
};

enum class invalidate_physics_bit {
    angles_changed = (1 << 1),
    animation_changed = (1 << 3),
    sequence_changed = (1 << 5)
};

struct player_info {
    uint64_t version{ };
    union {
        uint64_t full;
        struct {
            uint32_t low;
            uint32_t high;
        };
    } xuid{ };
    char               name[128]{ };
    int                uid{ };
    char               guid[33]{ };
    uint32_t           friends_id{ };
    char               friends_name[128]{ };
    bool               is_fake_player{ };
    bool               is_hltv{ };
    int                custom_files[4]{ };
    unsigned char      files_downloaded{ };
};

struct base_combat_character : public base_animating {
    NETVAR(get_next_attack, float, "CBaseCombatCharacter->m_flNextAttack")
    NETVAR(get_active_weapon_handle, base_handle, "CBaseCombatCharacter->m_hActiveWeapon")
    PTR_NETVAR(get_weapons, base_handle, "CBaseCombatCharacter->m_hMyWeapons")
    PTR_NETVAR(get_wearables, base_handle, "CBaseCombatCharacter->m_hMyWearables")
};

struct base_player : public base_combat_character {
    NETVAR(get_viewpunch_angle, angle, "CBasePlayer->m_viewPunchAngle")
    NETVAR(get_aimpunch_angle, angle, "CBasePlayer->m_aimPunchAngle")
    NETVAR(get_aimpunch_angle_velocity, vec3, "CBasePlayer->m_aimPunchAngleVel")
    NETVAR(get_view_offset, vec3, "CBasePlayer->m_vecViewOffset[0]")
    NETVAR(get_tick_base, int, "CBasePlayer->m_nTickBase")
    NETVAR(get_next_think_tick, int, "CBasePlayer->m_nNextThinkTick")
    NETVAR(get_fall_velocity, float, "CBasePlayer->m_flFallVelocity")
    NETVAR(get_health, int, "CBasePlayer->m_iHealth")
    NETVAR(get_life_state, life_state, "CBasePlayer->m_lifeState")
    NETVAR(get_ammo, int, "CBasePlayer->m_iAmmo")
    NETVAR(get_flags, bitfield<entity_flag>, "CBasePlayer->m_fFlags")
    NETVAR(get_observer_mode, observer_mode, "CBasePlayer->m_iObserverMode")
    NETVAR(get_observer_target_handle, base_handle, "CBasePlayer->m_hObserverTarget")
    NETVAR(get_viewmodel_handle, base_handle, "CBasePlayer->m_hViewModel[0]")
    NETVAR(get_duck_amount, float, "CBasePlayer->m_flDuckAmount")
    NETVAR(get_duck_speed, float, "CBasePlayer->m_flDuckSpeed")
    NETVAR(get_water_level, int, "CBasePlayer->m_nWaterLevel")

    DATAMAP_FIELD(get_collision_state, int, get_pred_desc_map(), "CBasePlayer->m_vphysicsCollisionState");
};

struct anim_state;

struct player : public base_player {
    NETVAR(get_money, int, "CCSPlayer->m_iAccount")
    NETVAR(get_total_hits, int, "CCSPlayer->m_totalHitsOnServer")
    NETVAR(get_shots_fired, int, "CCSPlayer->m_iShotsFired")
    NETVAR(has_helmet, bool, "CCSPlayer->m_bHasHelmet")
    NETVAR(has_heavy_armor, bool, "CCSPlayer->m_bHasHeavyArmor")
    NETVAR(has_defuser, bool, "CCSPlayer->m_bHasDefuser")
    NETVAR(is_immune, bool, "CCSPlayer->m_bGunGameImmunity")
    NETVAR(is_in_buy_zone, bool, "CCSPlayer->m_bInBuyZone")
    NETVAR(get_move_state, move_state, "CCSPlayer->m_iMoveState")
    NETVAR(is_scoping, bool, "CCSPlayer->m_bIsScoped")
    NETVAR(get_armor, int, "CCSPlayer->m_ArmorValue")
    NETVAR(get_flash_duration, float, "CCSPlayer->m_flFlashDuration")
    NETVAR(get_lby, float, "CCSPlayer->m_flLowerBodyYawTarget")
    NETVAR(get_velocity_modifier, float, "CCSPlayer->m_flVelocityModifier")
    PTR_NETVAR(get_max_flash_alpha, float, "CCSPlayer->m_flFlashMaxAlpha")

    NETVAR_OFFSET(get_flash_alpha, float, "CCSPlayer->m_flFlashMaxAlpha", -0x8)
    NETVAR_OFFSET(get_anim_state, anim_state*, "CCSPlayer->m_bIsScoped", -0x14)

    VIRTUAL_FUNCTION(get_eye_angles, angle&, 169, (), (this))
    VIRTUAL_FUNCTION(set_local_view_angles, void, 373, (const angle& angles), (this, std::cref(angles)))

    VIRTUAL_FUNCTION_SIG(post_think_v_physics, void, dll::client, "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB", (this))
    VIRTUAL_FUNCTION_SIG(simulate_player_simulated_entities, void, dll::client, "56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74", (this))
    VIRTUAL_FUNCTION_SIG(invalidate_physics_recursive, void, dll::client,
        "55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56", (this, invalidate_physics_flags), int invalidate_physics_flags)

    bool is_enemy(player* other = nullptr) noexcept;
};

struct local_player : public player {
    user_cmd* cur_cmd{ };
    angle view{ };

    bool update() noexcept;
};

}
