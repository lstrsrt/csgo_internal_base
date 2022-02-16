#pragma once

namespace cs {

enum class draw_model_flag {
    entire_model,
    opaque_only = (1 << 0),
    translucent_only = (1 << 1),
    group_mask = (opaque_only | translucent_only),
    no_flexes = (1 << 2),
    static_lighting = (1 << 3),
    accuratetime = (1 << 4),
    no_shadows = (1 << 5),
    get_perf_stats = (1 << 6),
    wireframe = (1 << 7),
    item_blink = (1 << 8),
    shadow_depth_texture = (1 << 9),
    ssao_depth_texture = (1 << 12),
    generate_stats = (1 << 15)
};

struct studio_seq_desc {
    int base_ptr{ };
    int label_index{ };
    int activity_name_index{ };
    int flags{ };
    int activity{ };
    int activity_weight{ };
    int event_count{ };
    int event_index{ };
    vec3 bb_min{ };
    vec3 bb_max{ };
    int blend_count{ };
    int anim_index{ };
    int movement_index{ };
    int group_size[2]{ };
    int param_index[2]{ };
    float param_start[2]{ };
    float param_end[2]{ };
    int param_parent{ };
    float fade_in_time{ };
    float fadeout_time{ };
    int local_entry_node{ };
    int local_exit_node{ };
    int node_flags{ };
    float entry_phase{ };
    float exit_phase{ };
    float last_frame{ };
    int next_sequence{ };
    int pose{ };
    int ik_rule_count{ };
    int auto_layer_count{ };
    int auto_layer_index{ };
    int weight_list_index{ };
    int pose_key_index{ };
    int ik_lock_count{ };
    int ik_lock_index{ };
    int key_value_index{ };
    int key_value_size{ };
    int cycle_pose_index{ };
    int activity_modifier_index{ };
    int activity_modifier_count{ };
    int anim_tag_index{ };
    int anim_tag_count{ };
    PAD(0xc)
};

struct studio_bbox {
    int bone{ };
    int group{ };
    vec3 bb_min{ };
    vec3 bb_max{ };
    int hitbox_name_index{ };
    angle offset_rotation{ };
    float capsule_radius{ };
};

struct studio_hitbox_set {
    int name_index{ };
    int hitbox_count{ };
    int hitbox_index{ };

    inline auto get_name() const noexcept
    {
        return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(this) + name_index);
    }

    inline auto get_hitbox(int i) const noexcept
    {
        return reinterpret_cast<studio_bbox*>(reinterpret_cast<uintptr_t>(this) + hitbox_index) + i;
    }
};

struct studio_bone {
    int name_index{ };
    int parent{ };
    int bone_controller[6]{ };
    vec3 position{ };
    vec4 quaternion{ };
    vec3 rotation{ };
    vec3 position_scale{ };
    vec3 rotation_scale{ };
    mat3x4 pose_to_bone{ };
    vec4 alignment{ };
    int flags{ };
    int procedure_type{ };
    int procedure_index{ };
    mutable int physics_bone{ };
    int surface_prop_index{ };
    int contents{ };
    int surface_prop_lookup{ };

    inline auto get_name() const noexcept
    {
        return reinterpret_cast<const char*>(reinterpret_cast<uintptr_t>(this) + name_index);
    }
};

struct studio_hdr_t {
    int id{ };
    int version{ };
    int checksum{ };
    char name[64]{ };
    int length{ };
    vec3 eye_position{ };
    vec3 illum_position{ };
    vec3 hull_min{ };
    vec3 hull_max{ };
    vec3 view_bb_min{ };
    vec3 view_bb_max{ };
    int flags{ };
    int bone_count{ };
    int bone_index{ };
    int bone_controller_count{ };
    int bone_controller_index{ };
    int hitbox_set_count{ };
    int hitbox_set_index{ };
    int local_anim_count{ };
	int local_anim_index{ };
	int local_seq_count{ };
	int local_seq_index{ };

    inline auto get_bone(int i) const noexcept
    {
        return reinterpret_cast<studio_bone*>(reinterpret_cast<uintptr_t>(this) + bone_index) + i;
    }

    inline auto get_hitbox_set(int i) const noexcept
    {
        return reinterpret_cast<studio_hitbox_set*>(reinterpret_cast<uintptr_t>(this) + hitbox_set_index) + i;
    }

    inline auto get_local_seq_desc(int i) const noexcept
    {
        return reinterpret_cast<studio_seq_desc*>(reinterpret_cast<uintptr_t>(this) + sizeof(studio_seq_desc) * i + local_seq_index);
    }
};

struct studio_hdr {
    studio_hdr_t* studio_hdr;
    void* virtual_model{ };
    void* softbody{ };
    mutable utl_vec<const studio_hdr_t*> studio_hdr_cache{ };
    mutable int frame_unlock_counter{ };
    int* p_frame_unlock_counter{ };
    PAD(0x8)
    utl_vec<int> bone_flags{ };
    utl_vec<int> bone_parent{ };
    activity_to_sequence_mapping* activity_to_sequence{ };
};

struct draw_model_info {
    PAD(0x18)
    i::renderable* client_entity{ };
};

struct draw_model_state {
    studio_hdr_t* studio_hdr{ };
    void* studio_hw_data{ };
    i::renderable* renderable{ };
    const mat3x4* bones{ };
};

}
