#pragma once

namespace cs {

enum class collision_group {
    none,
    breakable_glass = 6,
    push_away = 17,
};

enum class surface_flag : uint16_t {
    no_draw = (1 << 7),
    hitbox = (1 << 15)
};

struct surface {
    const char* name{ };
    int16_t surface_props{ };
    bitfield<surface_flag> flags{ };
};

struct ray {
    // All of these are actually 4-dimensional, but w is unneeded so they're all padded.
    vec3 start{ };
    PAD(0x4)
    vec3 delta{ };
    PAD(0x4)
    vec3 start_offset{ };
    PAD(0x4)
    vec3 extents{ };
    PAD(0x4)
    const mat3x4* world_axis_transform{ };
    bool is_ray{ true };
    bool is_swept{ };

    ray(const vec3& start, const vec3& end, const vec3& mins, const vec3& maxs) noexcept
        : delta(end - start), start_offset(mins + maxs), extents(maxs - mins)
    {
        is_swept = (delta.x || delta.y || delta.z);
        extents *= 0.5f;
        is_ray = (extents.length_sqr() < 1e-6);
        start_offset *= 0.5f;
        this->start = start + start_offset;
        start_offset.inverse();
    };

    ray(const vec3& start, const vec3& end) noexcept
        : start(start), delta(end - start)
    {
        is_swept = (delta.x || delta.y || delta.z);
        start_offset.clear();
        extents.clear();
    };
};

using contents_t = int;

namespace contents {

    constexpr contents_t empty = 0;
    constexpr contents_t solid = (1 << 0);
    constexpr contents_t window = (1 << 1);
    constexpr contents_t grate = (1 << 3);
    constexpr contents_t slime = (1 << 4);
    constexpr contents_t water = (1 << 5);
    constexpr contents_t moveable = (1 << 14);
    constexpr contents_t playerclip = (1 << 16);
    constexpr contents_t monster = (1 << 25);
    constexpr contents_t debris = (1 << 26);
    constexpr contents_t hitbox = (1 << 30);

}

using trace_mask_t = int;

namespace trace_mask {

    constexpr trace_mask_t all = -1;
    constexpr trace_mask_t solid = (contents::solid | contents::moveable | contents::window | contents::monster | contents::grate);
    constexpr trace_mask_t player_solid = (contents::solid | contents::moveable | contents::playerclip | contents::window
        | contents::monster | contents::grate);
    constexpr trace_mask_t water = (contents::water | contents::moveable | contents::slime);
    constexpr trace_mask_t shot = (contents::solid | contents::moveable | contents::monster | contents::window | contents::debris 
        | contents::grate | contents::hitbox);
    constexpr trace_mask_t shot_brush_only = (contents::solid | contents::moveable | contents::window | contents::debris);
    constexpr trace_mask_t shot_hull = (contents::solid | contents::moveable | contents::monster | contents::window | contents::debris 
        | contents::grate);
    constexpr trace_mask_t solid_brush_only = (contents::solid | contents::moveable | contents::window | contents::grate);
    constexpr trace_mask_t player_solid_brush_only = (contents::solid | contents::moveable | contents::window | contents::playerclip 
        | contents::grate);

}

struct trace {
    vec3 start_pos{ };
    vec3 end_pos{ };
    vec3 plane{ };
    PAD(0x8)
    float fraction{ };
    int contents{ };
    uint16_t disp_flags{ };
    bool all_solid{ };
    bool start_solid{ };
    float fraction_left_solid{ };
    surface	surface{ };
    hitgroup hitgroup{ };
    PAD(0x4)
    base_entity* hit_entity{ };
    hitbox hitbox{ };

    inline int get_entity_index() const noexcept
    {
        return (hit_entity ? hit_entity->get_index() : -1);
    };

    inline bool did_hit() const noexcept
    {
        return (fraction < 1.0f || all_solid || start_solid);
    };

    inline bool did_hit_world() const noexcept
    {
        return !get_entity_index();
    };

    inline bool did_hit_non_world_entity() const noexcept
    {
        return (hit_entity != nullptr && !did_hit_world());
    };
};

enum class trace_type {
    everything,
    world_only,
    entities_only,
    everything_filter_props
};

struct trace_filter {
    virtual bool should_hit_entity(base_entity* entity, int mask) const = 0;
    virtual trace_type get_trace_type() const = 0;
};

struct trace_filter_skip_two_entities : public trace_filter {
    base_entity* first{ }, *second{ };

    trace_filter_skip_two_entities() noexcept = default;
    trace_filter_skip_two_entities(base_entity* first, base_entity* second) noexcept
        : first(first), second(second) {};

    inline bool should_hit_entity(base_entity* entity, trace_mask_t mask) const noexcept override
    {
        return entity != first && entity != second;
    }

    inline trace_type get_trace_type() const noexcept override
    {
        return trace_type::everything;
    }
};

struct trace_filter_world_only : public trace_filter {
    inline bool should_hit_entity(base_entity* entity, trace_mask_t mask) const noexcept override
    {
        return false;
    }

    inline trace_type get_trace_type() const noexcept override
    {
        return trace_type::world_only;
    }
};

struct trace_filter_world_and_props_only : public trace_filter {
    inline bool should_hit_entity(base_entity* entity, trace_mask_t mask) const noexcept override
    {
        return false;
    }

    inline trace_type get_trace_type() const noexcept override
    {
        return trace_type::everything;
    }
};

struct trace_filter_no_players : public trace_filter {
    base_entity* skip{ };
    trace_type type{ };

    trace_filter_no_players() noexcept = default;
    trace_filter_no_players(base_entity* skip, trace_type type = trace_type::everything) noexcept
        : skip(skip), type(type) {};

    inline bool should_hit_entity(base_entity* entity, trace_mask_t mask) const noexcept override
    {
        return false;
    }

    inline trace_type get_trace_type() const noexcept override
    {
        return type;
    }
};

struct trace_entity : public trace_filter {
    base_entity* hit{ };

    trace_entity() noexcept = default;
    trace_entity(base_entity* hit) noexcept
        : hit(hit) {};

    inline bool should_hit_entity(base_entity* entity, trace_mask_t mask) const noexcept override
    {
        return entity == hit;
    }

    inline trace_type get_trace_type() const noexcept override 
    {
        return trace_type::entities_only;
    }
};

using should_hit_fn = bool(*)(base_entity*, contents_t);

struct trace_filter_simple : public trace_filter {
    const base_entity* pass_entity{ };
    collision_group group{ };
    should_hit_fn extra_should_hit_check{ };

    trace_filter_simple() noexcept = default;
    trace_filter_simple(const base_entity* pass_entity, collision_group group, should_hit_fn extra_should_hit_check = nullptr) noexcept
        : pass_entity(pass_entity), group(group), extra_should_hit_check(extra_should_hit_check) {};

    inline bool should_hit_entity(base_entity* entity, int mask) const noexcept override
    {
        return false;
    }

    inline trace_type get_trace_type() const noexcept override
    {
        return trace_type::everything;
    }
};

}
