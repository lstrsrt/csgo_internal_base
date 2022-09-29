#pragma once

#include "../../memory/memory.h"
#include "utl_vector.h"

namespace cs {

constexpr int max_pose_param = 24;
constexpr int max_anim_layers = 15;
constexpr int max_bones = 256;

struct player;
struct studio_seq_desc;
struct studio_hdr;
struct studio_hdr_t;
struct weapon;

enum class anim_layer {
    aim_matrix,
    weapon_action,
    weapon_action_recrouch,
    adjust,
    movement_jump_or_fall,
    movement_land_or_climb,
    movement_move,
    movement_strafe_change,
    whole_body,
    flashed,
    flinch,
    alive_loop,
    lean
};

enum class anim_layer_activity {
    die_stand = 953,
    die_stand_headshot,
    die_crouch,
    die_crouch_headshot,
    null,
    defuse,
    defuse_with_kit,
    flashbang_reaction,
    fire_primary,
    fire_primary_opt_1,
    fire_primary_opt_2,
    fire_secondary,
    fire_secondary_opt_1,
    fire_secondary_opt_2,
    reload,
    reload_start,
    reload_loop,
    reload_end,
    operate,
    deploy,
    _catch,
    silencer_detach,
    silencer_attach,
    twitch,
    twitch_buyzone,
    plant_bomb,
    idle_turn_balance_adjust,
    idle_adjust_stopped_moving,
    alive_loop,
    flinch,
    flinch_head,
    flinch_molotov,
    jump,
    fall,
    climb_ladder,
    land_light,
    land_heavy,
    exit_ladder_top,
    exit_ladder_bottom
};

enum class pose_param {
    lean_yaw,
    speed,
    ladder_speed,
    ladder_yaw,
    move_yaw,
    run,
    body_yaw,
    body_pitch,
    death_yaw,
    stand,
    jump_fall,
    aim_blend_stand_idle,
    aim_blend_crouch_idle,
    strafe_dir,
    aim_blend_stand_walk,
    aim_blend_stand_run,
    aim_blend_crouch_walk,
    move_blend_walk,
    move_blend_run,
    move_blend_crouch_walk
};

using pose_params = std::array<float, max_pose_param>;

struct base_animating;

struct anim_layer_t {
    float layer_anim_time{ };
    float layer_fadeout_time{ };
    studio_hdr* dispatched_studio_hdr{ };
    int dispatched_src{ };
    int dispatched_dest{ };
    int order{ };
    int sequence{ };
    float prev_cycle{ };
    float weight{ };
    float weight_delta_rate{ };
    float playback_rate{ };
    float cycle{ };
    base_animating* owner{ };
    int invalidate_physics_bits{ };
};

struct anim_state {
    VIRTUAL_FUNCTION_SIG(reset, void, dll::client, "56 6A 01 68 ? ? ? ? 8B F1", (this))
    VIRTUAL_FUNCTION_SIG(update, void, dll::client, "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24", (this, eye_pitch, eye_yaw, force),
        float eye_yaw, float eye_pitch, bool force)

    int* layer_order_preset{ };
    bool first_run_since_init{ };
    PAD(0x5b)
    base_animating* entity{ };
    weapon* cur_weapon{ };
    weapon* last_weapon{ };
    float last_update_time{ };
    int last_update_frame{ };
    float last_update_increment{ };
    float eye_yaw{ };
    float eye_pitch{ };
    float foot_yaw{ };
    float last_foot_yaw{ };
    float move_yaw{ };
    float move_yaw_ideal{ };
    float move_yaw_cur_to_ideal{ };
    PAD(0x4)
    float primary_cycle{ };
    float move_weight{ };
    PAD(0x4)
    float anim_duck_amount{ };
    float duck_additional{ };
    PAD(0x4)
    vec3 cur_position{ };
    vec3 last_position{ };
    vec3 velocity{ };
    vec3 velocity_normalized{ };
    vec3 velocity_normalized_non_zero{ };
    float velocity_length_xy{ };
    float velocity_length_z{ };
    float speed_as_portion_of_run_top_speed{ };
    float speed_as_portion_of_walk_top_speed{ };
    float speed_as_portion_of_crouch_top_speed{ };
    float duration_moving{ };
    float duration_still{ };
    bool is_on_ground{ };
    bool is_landing{ };
    PAD(0x6)
    float duration_in_air{ };
    float left_ground_height{ };
    float land_anim_multiplier{ };
    float walk_to_run_transition{ };
    PAD(0x4)
    float in_air_smooth_value{ };
    bool is_on_ladder{ };
    PAD(0x2f)
    float last_velocity_test_time{ };
    vec3 last_velocity{ };
    vec3 target_acceleration{ };
    vec3 acceleration{ };
    float acceleration_weight{ };
    PAD(0xc)
    float strafe_change_weight{ };
    PAD(0x4)
    float strafe_change_cycle{ };
    int strafe_sequence{ };
    PAD(0x184)
    float camera_smooth_height{ };
    bool smooth_height_valid{ };
    PAD(0xb)
    float aim_yaw_min{ };
    float aim_yaw_max{ };
    float aim_pitch_min{ };
    float aim_pitch_max{ };
    int anim_state_version{ };
};

struct activity_to_sequence_mapping {
    VIRTUAL_FUNCTION_SIG(select_weighted_sequence_from_modifiers, int, dll::server, "55 8B EC 83 EC 2C 53 56 8B 75 08 8B D9",
        (this, hdr, activity, modifiers, modifier_count),
        studio_hdr* hdr, anim_layer_activity activity, void* modifiers, int modifier_count)
};

struct ik_target {
    int frame_count{ };
    PAD(0x51)
};

struct ik_context {
    VIRTUAL_FUNCTION_SIG(construct, void, dll::client,
        "53 8B D9 F6 C3 03 74 0B FF 15 ?? ?? ?? ?? 84 C0 74 01 CC C7 83 ?? ?? ?? ?? ?? ?? ?? ?? 8B CB", (this))
    VIRTUAL_FUNCTION_SIG(destruct, void, dll::client,
        "56 8B F1 57 8D 8E ?? ?? ?? ?? E8 ?? ?? ?? ?? 8D 8E ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 BE", (this))
    VIRTUAL_FUNCTION_SIG(init, void, dll::client, "55 8B EC 83 EC ? 8B 45 ? 56 57 8B F9 8D 8F",
        (this, hdr, angles, pos, time, frame_counter, bone_mask),
        studio_hdr* hdr, angle& angles, vec3& pos, float time, int frame_counter, int bone_mask)
    VIRTUAL_FUNCTION_SIG(add_dependencies, void, dll::server, "55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 0F 28 CB F3 0F 11 4D",
        (this, seq_desc, sequence, cycle, pose_params, weight),
        studio_seq_desc& seq_desc, int sequence, float cycle, const float* pose_params, float weight)
    VIRTUAL_FUNCTION_SIG(solve_dependencies, void, dll::client, "55 8B EC 83 E4 ? 81 EC ? ? ? ? 8B 81 ? ? ? ? 56 57",
        (this, pos, q, bone_to_world, bone_computed), vec3* pos, vec4* q, mat3x4* bone_to_world, uint8_t* bone_computed)
    VIRTUAL_FUNCTION_SIG(update_targets, void, dll::client, "55 8B EC 83 E4 ? 81 EC ? ? ? ? 33 D2 89 4C 24 ? 56 57",
        (this, pos, q, bone_to_world, bone_computed), vec3* pos, vec4* q, mat3x4* bone_to_world, uint8_t* bone_computed)
    VIRTUAL_FUNCTION_SIG(copy_to, void, dll::server, "55 8B EC 83 EC 24 8B 45 08 57 8B F9 89 7D F4 85 C0",
        (this, other, remapping), ik_context* other, const unsigned short* remapping)
};

enum class bone_flag {
    always_procedural = (1 << 2),
    used_by_anything = 0xfff00,
    used_by_hitbox = (1 << 8),
    used_by_bone_merge = (1 << 18)
};

struct bone_cache {
    mat3x4* cached_bones{ };
    PAD(0x8)
    uint32_t count{ };
};

struct bone_merge_cache {
    struct merged_bone {
        unsigned short my_bone{ };
        unsigned short parent_bone{ };
    };

    base_animating* owner{ };
    base_animating* follow{ };
    studio_hdr* follow_hdr{ };
    const studio_hdr_t* follow_render_hdr{ };
    studio_hdr* owner_hdr{ };
    const studio_hdr_t* owner_render_hdr{ };
    int copied_framecount{ };
    bitfield<bone_flag> follow_bone_setup_mask{ };
    pose_param owner_to_follow_pose_param_mapping[max_pose_param]{ };
    utl_vec<merged_bone> merged_bones{ };
    uint32_t* bone_merge_bits{ };
    unsigned short raw_index_mapping[max_bones]{ };
    bool force_cache_clear{ };
};

struct bone_accessor {
    base_animating* animating{ };
    mat3x4* bones{ };
    int readable_bones{ };
    int writable_bones{ };
};

struct bone_setup {
    void init_pose(const studio_hdr* hdr, vec3* pos, vec4* q, int bone_mask) noexcept
    {
        static void(* init_pose_fn)(const studio_hdr*, vec3*, vec4*, int) =
            memory::find_bytes(dll::client, PATTERN("55 8B EC 83 EC 10 53 8B D9 89 ? ? 56 57 89")).cast<decltype(init_pose_fn)>();
        return init_pose_fn(hdr, pos, q, bone_mask);
    }

    VIRTUAL_FUNCTION_SIG(accumulate_pose, void, dll::client, "55 8B EC 83 E4 ? B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 56 57 8B F9",
        (this, pos, q, sequence, cycle, weight, time, ik_ctx),
        vec3* pos, vec4* q, int sequence, float cycle, float weight, float time, ik_context* ik_ctx)
};

}
