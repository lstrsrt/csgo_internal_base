#pragma once

#include "../../core/netvars.h"
#include "../../memory/memory.h"

#include "animations.h"
#include "client_class.h"
#include "cmd.h"
#include "utl_vector.h"
#include "view.h"

namespace cs {

struct base_entity;
struct model;
struct client_class;
struct datamap;
struct ray;
struct trace;

enum class solid_type {
    none,
    bsp,
    bbox,
    obb,
    obb_yaw,
    custom,
    vphysics
};

enum class solid_flag {
    customraytest = (1 << 0),
    customboxtest = (1 << 1),
    not_solid = (1 << 2),
    trigger = (1 << 3),
    not_standable = (1 << 4),
    volume_contents = (1 << 5),
    force_world_aligned = (1 << 6),
    use_trigger_bounds = (1 << 7),
    root_parent_aligned = (1 << 8),
    trigger_touch_debris = (1 << 9),
    trigger_touch_player = (1 << 10),
    not_moveable = (1 << 11)
};

enum class data_update_type {
    created,
    datatable_changed,
    post_update
};

}

namespace i {

struct unknown;

struct collideable {
    virtual void* get_entity_handle() = 0;
    virtual vec3& obb_mins() const = 0;
    virtual vec3& obb_maxs() const = 0;
    virtual void world_space_trigger_bounds(vec3* mins, vec3* maxs) const = 0;
    virtual bool test_collision(const cs::ray& ray, unsigned int mask, cs::trace& trace) = 0;
    virtual bool test_hitboxes(const cs::ray& ray, unsigned int mask, cs::trace& trace) = 0;
    virtual int get_collision_model_index() = 0;
    virtual const cs::model* get_collision_model() = 0;
    virtual const vec3& get_collision_origin() const = 0;
    virtual const angle& get_collision_angles() const = 0;
    virtual const mat3x4& collision_to_world_transform() const = 0;
    virtual cs::solid_type get_solid_type() const = 0;
    virtual int get_solid_flags() const = 0;
    virtual unknown* get_client_unknown() = 0;
    virtual int get_collision_group() const = 0;
};

struct renderable {
    virtual unknown* get_client_unknown() = 0;
    virtual vec3 const& get_render_origin() = 0;
    virtual angle const& get_render_angles() = 0;
    virtual bool should_draw() = 0;
    virtual int get_render_flags() = 0;
    virtual void is_transparent() = 0;
    virtual uint16_t get_shadow_handle() const = 0;
    virtual uint16_t& get_render_handle() = 0;
    virtual const cs::model* get_model() const = 0;
    virtual int draw_model(int flags, uint8_t alpha) = 0;
    virtual int get_body() = 0;
    virtual void get_color_modulation(float* color) = 0;
    virtual bool lod_test() = 0;
    virtual bool setup_bones(mat3x4* bones, int max, int mask, float time) = 0;
    virtual void do_animation_events() = 0;
    virtual void* get_pvs_notify_interface() = 0;
    virtual void get_render_bounds(vec3& mins, vec3& maxs) = 0;
};

struct networkable {
    virtual unknown* get_client_unknown() = 0;
    virtual void release() = 0;
    virtual cs::client_class* get_client_class() = 0;
    virtual void notify_should_transmit(int state) = 0;
    virtual void on_pre_data_changed(cs::data_update_type update_type) = 0;
    virtual void on_data_changed(cs::data_update_type update_type) = 0;
    virtual void pre_data_update(cs::data_update_type update_type) = 0;
    virtual void post_data_update(cs::data_update_type update_type) = 0;
    virtual void on_data_unchanged_in_pvs() = 0;
    virtual bool is_dormant() const = 0;
    virtual int	get_index() const = 0;
    virtual void receive_message(cs::class_id class_index, void* msg) = 0;
    virtual void* get_data_table_base_ptr() = 0;
    virtual void set_destroyed_on_recreate_entities() = 0;
};

struct thinkable {
    virtual unknown* get_client_unknown() = 0;
    virtual void client_think() = 0;
    virtual void* think_handle() = 0;
    virtual void set_think_handle(void* think) = 0;
    virtual void release() = 0;
};

struct client_entity;

struct unknown {
    // IHandleEntity
    virtual	~unknown() = default;
    virtual void set_ref_handle(const int& handle) = 0;
    virtual const int& get_ref_handle() const = 0;
    // IClientUnknown
    virtual collideable* get_collideable() = 0;
    virtual networkable* get_networkable() = 0;
    virtual renderable* get_renderable() = 0;
    virtual client_entity* get_client_entity() = 0;
    virtual cs::base_entity* get_base_entity() = 0;
    virtual thinkable* get_thinkable() = 0;
    virtual void* get_alpha_property() = 0;
};

struct client_entity : public unknown, public renderable, public networkable, public thinkable {
    VIRTUAL_FUNCTION(get_data_desc_map, cs::datamap*, 15, (), (this))
    VIRTUAL_FUNCTION(get_pred_desc_map, cs::datamap*, 17, (), (this))
};

}

namespace cs {

using base_handle = unsigned long;

enum class item_id {
    weapon_none = 0,
    desert_eagle,
    dual_berettas,
    fiveseven,
    glock18,
    ak47 = 7,
    aug,
    awp,
    famas,
    g3sg1,
    galil_ar = 13,
    m249,
    m4a4 = 16,
    mac10,
    p90 = 19,
    repulsor_device,
    mp5sd = 23,
    ump45,
    xm1014,
    ppbizon,
    mag7,
    negev,
    sawedoff,
    tec9,
    zeus_x27,
    p2000,
    mp7,
    mp9,
    nova,
    p250,
    ballistic_shield,
    scar20,
    sg_553,
    ssg_08,
    knife_gold,
    knife_default_ct,
    grenade_flashbang,
    grenade_he,
    grenade_smoke,
    grenade_molotov,
    grenade_decoy,
    grenade_incendiary,
    c4,
    kevlar,
    kevlar_helmet,
    heavy_assault_suit,
    defuse_kit = 55,
    rescue_kit,
    medishot,
    knife_default_t = 59,
    m4a1s,
    usps,
    cz75a = 63,
    r8_revolver,
    grenade_ta = 68,
    bare_hands,
    breach_charge,
    tablet = 72,
    melee = 74,
    axe,
    hammer,
    wrench = 78,
    knife_spectral_shiv = 80,
    grenade_fire_bomb,
    grenade_diversion_device,
    grenade_frag,
    snowball,
    bump_mine,
    knife_bayonet = 500,
    knife_classic = 503,
    knife_flip = 505,
    knife_gut,
    knife_karambit,
    knife_m9_bayonet,
    knife_huntsman,
    knife_falchion = 512,
    knife_bowie = 514,
    knife_butterfly,
    knife_shadow_daggers,
    knife_paracord,
    knife_survival,
    knife_ursus,
    knife_navaja,
    knife_nomad,
    knife_stiletto,
    knife_talon,
    knife_skeleton = 525,
    glove_brokenfang = 4725,
    glove_bloodhound = 5027,
    glove_default_t,
    glove_default_ct,
    glove_sport,
    glove_driver,
    glove_hand_wraps,
    glove_moto,
    glove_specialist,
    glove_hydra,
    agent_t_map_based,
    agent_ct_map_based,
    agent_t_anarchist,
    agent_t_anarchist_a,
    agent_t_anarchist_b,
    agent_t_anarchist_c,
    agent_t_anarchist_d,
    agent_t_pirate,
    agent_t_pirate_a,
    agent_t_pirate_b,
    agent_t_pirate_c,
    agent_t_pirate_d,
    agent_t_professional,
    agent_t_professional_a,
    agent_t_professional_b,
    agent_t_professional_c,
    agent_t_professional_d,
    agent_t_separatist,
    agent_t_separatist_a,
    agent_t_separatist_b,
    agent_t_separatist_c,
    agent_t_separatist_d,
    agent_ct_gign,
    agent_ct_gign_a,
    agent_ct_gign_b,
    agent_ct_gign_c,
    agent_ct_gign_d,
    agent_ct_gsg9,
    agent_ct_gsg9_a,
    agent_ct_gsg9_b,
    agent_ct_gsg9_c,
    agent_ct_gsg9_d,
    agent_ct_idf,
    agent_ct_idf_b,
    agent_ct_idf_c,
    agent_ct_idf_d,
    agent_ct_idf_e,
    agent_ct_idf_f,
    agent_ct_swat,
    agent_ct_swat_a,
    agent_ct_swat_b,
    agent_ct_swat_c,
    agent_ct_swat_d,
    agent_ct_sas_a,
    agent_ct_sas_b,
    agent_ct_sas_c,
    agent_ct_sas_d,
    agent_ct_st6,
    agent_ct_st6_a,
    agent_ct_st6_b,
    agent_ct_st6_c,
    agent_ct_st6_d,
    agent_t_balkan,
    agent_t_balkan_a,
    agent_t_balkan_b,
    agent_t_balkan_c,
    agent_t_balkan_d,
    agent_t_jumpsuit_a,
    agent_t_jumpsuit_b,
    agent_t_jumpsuit_c,
    agent_t_heavy,
    agent_ct_heavy,
    agent_t_leet_a = 5100,
    agent_t_leet_b,
    agent_t_leet_c,
    agent_t_leet_d,
    agent_t_leet_e,
    agent_t_elite_crew_ground_rebel,
    agent_t_elite_crew_osiris,
    agent_t_elite_crew_prof_shahmat,
    agent_t_elite_crew_mr_muhlik,
    agent_t_phoenix = 5200,
    agent_t_phoenix_a,
    agent_t_phoenix_b,
    agent_t_phoenix_c,
    agent_t_phoenix_d,
    agent_t_phoenix_soldier,
    agent_t_phoenix_enforcer,
    agent_t_phoenix_slingshot,
    agent_t_phoenix_i,
    agent_ct_fbi = 5300,
    agent_ct_fbi_a,
    agent_ct_fbi_c,
    agent_ct_fbi_d,
    agent_ct_fbi_e,
    agent_ct_fbi_f,
    agent_ct_fbi_g,
    agent_ct_fbi_h,
    agent_ct_fbi_b,
    agent_ct_st6_k = 5400,
    agent_ct_st6_e,
    agent_ct_st6_g,
    agent_ct_st6_m,
    agent_ct_st6_i,
    agent_ct_st6_j = 4619,
    agent_ct_st6_l = 4680,
    agent_t_balkan_f = 5500,
    agent_t_balkan_i,
    agent_t_balkan_g,
    agent_t_balkan_j,
    agent_t_balkan_h,
    agent_t_balkan_k = 4718,
    agent_t_balkan_l = 5505,
    agent_ct_sas = 5600,
    agent_ct_sas_b_squadron_officer,
    agent_ct_swat_e = 4711,
    agent_ct_swat_f,
    agent_ct_swat_g,
    agent_ct_swat_h,
    agent_ct_swat_i,
    agent_ct_swat_j,
    agent_t_professional_e = 4726,
    agent_t_professional_f_1 = 4733,
    agent_t_professional_f_2,
    agent_t_professional_f_3,
    agent_t_professional_f_4,
    agent_t_professional_g = 4727,
    agent_t_professional_h,
    agent_t_professional_i = 4732,
    agent_t_professional_j = 4730
};

enum class weapon_type {
    knife,
    pistol,
    smg,
    rifle,
    shotgun,
    sniper,
    machinegun,
    c4,
    grenade = 9,
    health_shot = 11,
    fists,
    breach_charge,
    bumpmine,
    tablet,
    melee
};

enum class entity_flag {
    on_ground = (1 << 0),
    ducking = (1 << 1),
    anim_ducking = (1 << 2),
    water_jump = (1 << 3),
    frozen = (1 << 6),
    in_water = (1 << 10),
    godmode = (1 << 15)
};

enum class eflag
{
    dirty_abs_transform = (1 << 11),
    dirty_abs_velocity = (1 << 12),
    dirty_abs_ang_velocity = (1 << 13),
    no_think_function = (1 << 22),
    no_game_physics_simulation = (1 << 23)
};

enum class effect {
    no_interpolation = (1 << 3)
};

enum class move_type {
    none,
    walk = 2,
    noclip = 8,
    ladder = 9,
    observer = 10
};

enum class hitbox {
    head,
    neck,
    pelvis,
    stomach,
    lower_chest,
    chest,
    upper_chest,
    right_thigh,
    left_thigh,
    right_calf,
    left_calf,
    right_foot,
    left_foot,
    right_hand,
    left_hand,
    right_upper_arm,
    right_forearm,
    left_upper_arm,
    left_forearm
};

enum class hitgroup {
    invalid = -1,
    generic,
    head,
    chest,
    stomach,
    left_arm,
    right_arm,
    left_leg,
    right_leg,
    gear = 10
};

enum class team {
    unassigned,
    spectator,
    t,
    ct
};

enum class think_method {
    all_functions
};

enum class interpolation_flag {
    latch_animation_var = (1 << 0),
    latch_simulation_var = (1 << 1)
};

struct studio_hdr;
struct weapon;
struct weapon_info;

struct base_entity : public i::client_entity {
    NETVAR(get_simulation_time, float, "CBaseEntity->m_flSimulationTime")
    NETVAR(get_anim_time, float, "CBaseEntity->m_flAnimTime")
    NETVAR(get_origin, vec3, "CBaseEntity->m_vecOrigin")
    NETVAR(get_rotation, angle, "CBaseEntity->m_angRotation")
    NETVAR(get_model_index, int, "CBaseEntity->m_nModelIndex")
    NETVAR(get_team, team, "CBaseEntity->m_iTeamNum")
    NETVAR(get_owner_entity_handle, base_handle, "CBaseEntity->m_hOwnerEntity")
    NETVAR(is_spotted, bool, "CBaseEntity->m_bSpotted")

    NETVAR_OFFSET(get_old_simulation_time, float, "CBaseEntity->m_flSimulationTime", 0x4)
    NETVAR_OFFSET(get_move_type, move_type, "CBaseEntity->m_nRenderMode", 0x1)
    NETVAR_OFFSET(get_coordinate_frame, mat3x4, "CBaseEntity->m_CollisionGroup", -0x30)
    
    OFFSET(get_effects, bitfield<effect>, 0xf0)

    DATAMAP_FIELD(get_button_last, bitfield<cmd_button>, get_pred_desc_map(), "m_afButtonLast")
    DATAMAP_FIELD(get_button_pressed, bitfield<cmd_button>, get_pred_desc_map(), "m_afButtonPressed")
    DATAMAP_FIELD(get_button_released, bitfield<cmd_button>, get_pred_desc_map(), "m_afButtonReleased")
    DATAMAP_FIELD(get_eflags, bitfield<entity_flag>, get_pred_desc_map(), "m_iEFlags")
    DATAMAP_FIELD(get_abs_rotation, angle, get_data_desc_map(), "m_angAbsRotation")
    DATAMAP_FIELD(get_abs_velocity, vec3, get_data_desc_map(), "m_angAbsVelocity")
    PTR_DATAMAP_FIELD(get_buttons, bitfield<cmd_button>, get_pred_desc_map(), "m_nButtons")

    VIRTUAL_FUNCTION(get_abs_origin, vec3&, 10, (), (this))
    VIRTUAL_FUNCTION(get_abs_angles, angle&, 11, (), (this))
    VIRTUAL_FUNCTION(set_model_index, void, 75, (int index), (this, index))
    VIRTUAL_FUNCTION(get_attachment, bool, 84, (int index, vec3* origin), (this, index, origin))
    VIRTUAL_FUNCTION(on_latch_interpolated_variables, void, 107, (bitfield<interpolation_flag> flags), (this, flags.raw()))
    VIRTUAL_FUNCTION(think, void, 138, (), (this))
    VIRTUAL_FUNCTION(is_alive, bool, 156, (), (this))
    VIRTUAL_FUNCTION(is_player, bool, 158, (), (this))
    VIRTUAL_FUNCTION(is_weapon, bool, 166, (), (this))
    VIRTUAL_FUNCTION(get_active_weapon, weapon*, 268, (), (this))
    VIRTUAL_FUNCTION(pre_think, void, 318, (), (this))
    VIRTUAL_FUNCTION(select_item, void, 330, (const char* name, int subtype), (this, name, subtype))
    VIRTUAL_FUNCTION(update_collision_bounds, void, 340, (), (this))
    VIRTUAL_FUNCTION(get_weapon_info, weapon_info*, 461, (), (this))

    VIRTUAL_FUNCTION_SIG(set_abs_origin, void, dll::client, "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8 ? ? ? ? 8B 7D", 
        (this, std::cref(origin)), const vec3& origin)
    VIRTUAL_FUNCTION_SIG(set_abs_angles, void, dll::client, "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1", 
        (this, std::cref(angles)), const angle& angles)
    VIRTUAL_FUNCTION_SIG(physics_run_think, bool, dll::client, "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87", 
        (this, method), think_method method)
};

struct base_attributable_item : public base_entity {
    NETVAR(get_item_id, short, "CBaseAttributableItem->m_iItemDefinitionIndex")
    NETVAR(get_account_id, int, "CBaseAttributableItem->m_iAccountID")
    NETVAR(get_quality, int, "CBaseAttributableItem->m_iEntityQuality")
    NETVAR(get_id_high, int, "CBaseAttributableItem->m_iItemIDHigh")
    NETVAR(get_id_low, int, "CBaseAttributableItem->m_iItemIDLow")
    NETVAR(get_xuid_low, int, "CBaseAttributableItem->m_OriginalOwnerXuidLow")
    NETVAR(get_xuid_high, int, "CBaseAttributableItem->m_OriginalOwnerXuidHigh")
    NETVAR(get_fallback_paintkit, int, "CBaseAttributableItem->m_nFallbackPaintKit")
    NETVAR(get_fallback_seed, int, "CBaseAttributableItem->m_nFallbackSeed")
    NETVAR(get_fallback_wear, float, "CBaseAttributableItem->m_flFallbackWear")
    NETVAR(get_fallback_stat_trak, int, "CBaseAttributableItem->m_nFallbackStatTrak")
};

struct base_view_model {
    NETVAR(get_model_index, int, "CBaseViewModel->m_nModelIndex")
    NETVAR(get_owner_handle, base_handle, "CBaseViewModel->m_hOwner")
    NETVAR(get_weapon_handle, base_handle, "CBaseViewModel->m_hWeapon")
};

struct base_animating : public base_entity {
    NETVAR(get_sequence, int, "CBaseAnimating->m_nSequence")
    NETVAR(get_pose_params, pose_params, "CBaseAnimating->m_flPoseParameter")
    NETVAR(use_client_side_animation, bool, "CBaseAnimating->m_bClientSideAnimation")

    OFFSET(get_ik_ctx, ik_context*, 0x266c)
    OFFSET(get_most_recent_model_bone_counter, unsigned long, 0x268c)
    OFFSET(get_bone_merge_cache, bone_merge_cache*, 0x2908)
    OFFSET(get_last_bone_setup_time, float, 0x2924)
    OFFSET(is_jiggle_bones_enabled, bool, 0x2928)
    OFFSET(get_studio_hdr, studio_hdr*, 0x294c)
    OFFSET(get_anim_layers, utl_vec<anim_layer_t>, 0x298c)
    PTR_OFFSET(get_bone_accessor, bone_accessor, 0x26a0)
    PTR_OFFSET(get_bone_cache, bone_cache, 0x2910)

    VIRTUAL_FUNCTION(update_ik_locks, void, 192, (float cur_time), (this, cur_time))
    VIRTUAL_FUNCTION(calculate_ik_locks, void, 193, (float cur_time), (this, cur_time))
    VIRTUAL_FUNCTION(set_sequence, void, 219, (int sequence), (this, sequence))
    VIRTUAL_FUNCTION(studio_frame_advance, void, 220, (), (this))
    VIRTUAL_FUNCTION(update_client_side_animation, void, 224, (), (this))
    VIRTUAL_FUNCTION(update_dispatch_layer, void, 247, (anim_layer_t* layer, studio_hdr* hdr, int sequence), (this, layer, hdr, sequence))
};

struct weapon_info {
    PAD(0x14)
    int max_clip1{ };
    int max_clip2{ };
    int default_clip1{ };
    int default_clip2{ };
    int primary_max_reserve_ammo{ };
    int secondary_max_reserve_ammo{ };
    char* world_model{ };
    char* view_model{ };
    char* dropped_model{ };
    PAD(0x50)
    char* hud_name{ };
    char* weapon_name{ };
    PAD(0x2)
    bool is_melee_weapon{ };
    PAD(0x9)
    float weapon_weight{ };
    PAD(0x28)
    weapon_type type { };
    PAD(0x4)
    int price{ };
    PAD(0x8)
    float cycle_time{ };
    PAD(0xc)
    bool full_auto{ };
    PAD(0x3)
    int damage{ };
    float headshot_multiplier{ };
    float armor_ratio{ };
    int bullets{ };
    float penetration{ };
    PAD(0x8)
    float range{ };
    float range_modifier{ };
    float throw_velocity{ };
    PAD(0xc)
    bool has_silencer{ };
    PAD(0xb)
    char* bullet_type{ };
    float max_speed{ };
    float max_speed_alt{ };
    float spread{ };
    float spread_alt{ };
    PAD(0x5c)
    float recoil_magnitude{ };
    float recoil_magnitude_alt{ };
    PAD(0x10)
    float recovery_time_stand{ };
};

struct weapon : public base_attributable_item {
    NETVAR(get_next_primary_attack, float, "CBaseCombatWeapon->m_flNextPrimaryAttack")
    NETVAR(get_next_secondary_attack, float, "CBaseCombatWeapon->m_flNextSecondaryAttack")
    NETVAR(get_ammo, int, "CBaseCombatWeapon->m_iClip1")
    NETVAR(get_reserve_ammo, int, "CBaseCombatWeapon->m_iPrimaryReserveAmmoCount")
    NETVAR(get_view_model_index, int, "CBaseCombatWeapon->m_iViewModelIndex")
    NETVAR(get_world_model_index, int, "CBaseCombatWeapon->m_iWorldModelIndex")
    NETVAR(get_world_model_handle, base_handle, "CBaseCombatWeapon->m_hWeaponWorldModel")

    NETVAR(get_accuracy_penalty, float, "CWeaponCSBase->m_fAccuracyPenalty")
    NETVAR(get_last_shot_time, float, "CWeaponCSBase->m_fLastShotTime")
    NETVAR(get_recoil_index, float, "CWeaponCSBase->m_flRecoilIndex")
    NETVAR(is_burst_mode, bool, "CWeaponCSBase->m_bBurstMode")
    NETVAR(get_fire_ready_time, float, "CWeaponCSBase->m_flPostponeFireReadyTime")

    NETVAR(get_zoom_level, int, "CWeaponCSBaseGun->m_zoomLevel")
    NETVAR(get_burst_shots_remaining, int, "CWeaponCSBaseGun->m_iBurstShotsRemaining")

    DATAMAP_FIELD(is_reloading, bool, get_pred_desc_map(), "m_bInReload")

    VIRTUAL_FUNCTION(get_spread, float, 453, (), (this))
    VIRTUAL_FUNCTION(get_inaccuracy, float, 483, (), (this))
    VIRTUAL_FUNCTION(update_accuracy_penalty, void, 484, (), (this))

    weapon_info* get_info() noexcept;
};

struct grenade : public base_entity {
    NETVAR(get_dmg_radius, float, "CBaseGrenade->m_DmgRadius")
    NETVAR(get_velocity, vec3, "CBaseGrenade->m_vecVelocity")
    NETVAR(get_thrower_handle, base_handle, "CBaseGrenade->m_hThrower")

    NETVAR(is_pin_pulled, bool, "CBaseCSGrenade->m_bPinPulled")
    NETVAR(get_throw_time, float, "CBaseCSGrenade->m_fThrowTime")
    NETVAR(get_throw_strength, float, "CBaseCSGrenade->m_flThrowStrength")
};

struct inferno : public base_entity {
    ARRAY_NETVAR(get_fire_x_delta, int, 100, "CInferno->m_fireXDelta")
    ARRAY_NETVAR(get_fire_y_delta, int, 100, "CInferno->m_fireYDelta")
    ARRAY_NETVAR(get_fire_z_delta, int, 100, "CInferno->m_fireZDelta")
    NETVAR(get_effect_tick_begin, int, "CInferno->m_nFireEffectTickBegin")
};

enum class bomb_site {
    a,
    b
};

struct planted_c4 : public base_entity {
    NETVAR(is_ticking, bool, "CPlantedC4->m_bBombTicking")
    NETVAR(get_blow_time, float, "CPlantedC4->m_flC4Blow")
    NETVAR(get_total_time, float, "CPlantedC4->m_flTimerLength")
    NETVAR(get_bomb_site, bomb_site, "CPlantedC4->m_nBombSite")
    NETVAR(is_defused, bool, "CPlantedC4->m_bBombDefused")
    NETVAR(get_defuse_countdown, float, "CPlantedC4->m_flDefuseCountDown")
    NETVAR(get_defuse_length, float, "CPlantedC4->m_flDefuseLength")
    NETVAR(get_defuser_handle, base_handle, "CPlantedC4->m_hBombDefuser")
    
    float get_remaining_time() noexcept;
    float get_remaining_defuse_time() noexcept;
};

enum class precipitation_type {
    rain,
    snow,
    ash,
    snowfall,
    particle_rain,
    particle_ash,
    particle_rainstorm,
    particle_snow
};

struct precipitation : public base_entity {
    NETVAR(get_type, precipitation_type, "CPrecipitation->m_nPrecipType")
};

struct tonemap_controller {
    NETVAR(use_custom_auto_exposure_min, bool, "CEnvTonemapController->m_bUseCustomAutoExposureMin")
    NETVAR(use_custom_auto_exposure_max, bool, "CEnvTonemapController->m_bUseCustomAutoExposureMax")
    NETVAR(use_custom_bloom_scale, bool, "CEnvTonemapController->m_bUseCustomBloomScale")
    NETVAR(get_custom_auto_exposure_min, float, "CEnvTonemapController->m_flCustomAutoExposureMin")
    NETVAR(get_custom_auto_exposure_max, float, "CEnvTonemapController->m_flCustomAutoExposureMax")
    NETVAR(get_custom_bloom_scale, float, "CEnvTonemapController->m_flCustomBloomScale")
    NETVAR(get_custom_bloom_scale_min, float, "CEnvTonemapController->m_flCustomBloomScaleMinimum")
    NETVAR(get_bloom_exponent, float, "CEnvTonemapController->m_flBloomExponent")
    NETVAR(get_bloom_saturation, float, "CEnvTonemapController->m_flBloomSaturation")
};

struct glow_object_definition {
    int next_free_slot{ };
    cs::base_entity* entity{ };
    vec3 color{ };
    float alpha{ };
    PAD(0x9)
    float pulse_overdrive;
    bool render_occluded{ true };
    bool render_unoccluded{ false };
    PAD(0x5)
    int style{ };

    static constexpr int end_of_free_list = -1;
    static constexpr int entry_in_use = -2;

    bool is_unused() const noexcept
    {
        return next_free_slot != entry_in_use; 
    }

    void set(const vec4& clr) noexcept 
    {
        color = { clr.x, clr.y, clr.z };
        alpha = clr.w;
    }
};

}
