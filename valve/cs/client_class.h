#pragma once

#include <type_traits>

namespace cs {

namespace i { struct networkable; }

enum class class_id {
    ai_base_npc,
    weapon_ak47,
    base_animating,
    base_animating_overlay,
    base_attributable_item,
    base_button,
    base_combat_character,
    base_combat_weapon,
    base_cs_grenade,
    base_cs_grenade_projectile,
    base_door,
    base_entity,
    base_flex,
    base_grenade,
    base_particle_entity,
    base_player,
    base_prop_door,
    base_team_objective_resource,
    base_temp_entity,
    base_toggle,
    base_trigger,
    base_viewmodel,
    base_vphysics_trigger,
    base_weapon_world_model,
    beam,
    beam_spotlight,
    bone_follower,
    br_c4_target,
    breach_charge,
    breach_charge_projectile,
    breakable_prop,
    breakable_surface,
    bump_mine,
    bump_mine_projectile,
    c4,
    cascade_light,
    chicken,
    color_correction,
    color_correction_volume,
    cs_gamerules_proxy,
    cs_player,
    cs_player_resource,
    cs_ragdoll,
    cs_team,
    dangerzone,
    dangerzone_controller,
    deagle,
    decoy_grenade,
    decoy_projectile,
    drone,
    drone_gun,
    dynamic_light,
    dynamic_prop,
    econ_entity,
    econ_wearable,
    embers,
    entity_dissolve,
    entity_flame,
    entity_freezing,
    entity_particle_trail,
    env_ambient_light,
    env_detail_controller,
    env_dof_controller,
    env_gas_canister,
    env_particle_script,
    env_projected_texture,
    env_quadratic_beam,
    env_screen_effect,
    env_screen_overlay,
    env_tonemap_controller,
    env_wind,
    fe_player_decal,
    firecracker_blast,
    fire_smoke,
    fire_trail,
    fish,
    fists,
    flashbang,
    fog_controller,
    footstep_control,
    func_dust,
    func_lod,
    func_area_portal_window,
    func_brush,
    func_conveyor,
    func_ladder,
    func_monitor,
    func_move_linear,
    func_occluder,
    func_reflective_glass,
    func_rotating,
    func_smoke_volume,
    func_track_train,
    gamerules_proxy,
    grass_burn,
    handle_test,
    he_grenade,
    hostage,
    hostage_carriable_prop,
    incendiary_grenade,
    inferno,
    info_ladder_dismount,
    info_map_region,
    info_overlay_accessor,
    item_healthshot,
    item_cash,
    item_dogtags,
    knife,
    knife_gg,
    light_glow,
    map_veto_pick_controller,
    material_modify_control,
    melee,
    molotov_grenade,
    molotov_projectile,
    movie_display,
    paradrop_chopper,
    particle_fire,
    particle_performance_monitor,
    particle_system,
    phys_box,
    phys_box_multiplayer,
    physics_prop,
    physics_prop_multiplayer,
    phys_magnet,
    phys_prop_ammo_box,
    phys_prop_loot_crate,
    phys_prop_radar_jammer,
    phys_prop_weapon_upgrade,
    planted_c4,
    plasma,
    player_ping,
    player_resource,
    point_camera,
    point_comment_arynode,
    point_world_text,
    pose_controller,
    post_process_controller,
    precipitation,
    precipitation_blocker,
    predicted_view_model,
    prop_hallucination,
    prop_counter,
    prop_door_rotating,
    prop_jeep,
    prop_vehicle_driveable,
    ragdoll_manager,
    ragdoll_prop,
    ragdoll_prop_attached,
    rope_keyframe,
    weapon_scar17,
    scene_entity,
    sensor_grenade,
    sensor_grenade_projectile,
    shadow_control,
    slide_show_display,
    smoke_grenade,
    smoke_grenade_projectile,
    smoke_stack,
    snowball,
    snowball_pile,
    snowball_projectile,
    spatial_entity,
    spotlight_end,
    sprite,
    sprite_oriented,
    sprite_trail,
    statue_prop,
    steamjet,
    sun,
    sunlight_shadow_control,
    survival_spawn_chopper,
    tablet,
    team,
    team_playround_based_rules_proxy,
    tear_morricochet,
    te_base_beam,
    te_beam_ent_point,
    te_beam_ents,
    te_beam_follow,
    te_beam_laser,
    te_beam_points,
    te_beam_ring,
    te_beam_ring_point,
    te_beam_spline,
    te_blood_sprite,
    te_blood_stream,
    te_break_model,
    te_bsp_decal,
    te_bubbles,
    te_bubble_trail,
    te_client_projectile,
    te_decal,
    te_dust,
    te_dynamic_light,
    te_effect_dispatch,
    te_energy_splash,
    te_explosion,
    te_fire_bullets,
    te_fizz,
    te_footprint_decal,
    te_foundry_helpers,
    te_gauss_explosion,
    te_glow_sprite,
    te_impact,
    te_kill_player_attachments,
    te_large_funnel,
    te_metal_sparks,
    te_muzzle_flash,
    te_particle_system,
    te_physics_prop,
    te_plant_bomb,
    te_player_anim_event,
    te_player_decal,
    te_projected_decal,
    te_radioicon,
    te_shatter_surface,
    te_show_line,
    te_sla,
    te_smoke,
    te_sparks,
    te_sprite,
    te_sprite_spray,
    test_proxy_toggle_networkable,
    test_trace_line,
    te_world_decal,
    trigger_player_movement,
    trigger_sound_operator,
    vgui_screen,
    vote_controller,
    water_bullet,
    water_lod_control,
    weapon_aug,
    weapon_awp,
    weapon_base_item,
    weapon_ppbizon,
    weapon_cs_base,
    weapon_cs_base_gun,
    weapon_cycler,
    weapon_dual_berettas,
    weapon_famas,
    weapon_fiveseven,
    weapon_g3sg1,
    weapon_galil,
    weapon_galil_ar,
    weapon_glock18,
    weapon_p2000,
    weapon_m249,
    weapon_m3,
    weapon_m4a4,
    weapon_mac10,
    weapon_mag7,
    weapon_mp5sd,
    weapon_mp7,
    weapon_mp9,
    weapon_negev,
    weapon_nova,
    weapon_p228,
    weapon_p250,
    weapon_p90,
    weapon_sawedoff,
    weapon_scar20,
    weapon_scout,
    weapon_sg550,
    weapon_sg552,
    weapon_sg553,
    weapon_ballistic_shield,
    weapon_ssg08,
    weapon_zeusx27,
    weapon_tec9,
    weapon_tmp,
    weapon_ump45,
    weapon_usp,
    weapon_xm1014,
    world,
    world_vgui_text,
    dust_trail,
    movie_explosion,
    particle_smoke_grenade,
    rocket_trail,
    smoke_trail,
    spore_explosion,
    spore_trail
};

struct recv_table;

struct client_class {
    std::add_pointer_t<i::networkable*(int, int)> create_client_class_fn{ };
    std::add_pointer_t<i::networkable*()> create_event_fn{ };
    const char* network_name{ };
    recv_table* recv_table{ };
    client_class* next{ };
    class_id id{ };
};

}
