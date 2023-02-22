#include "math.h"
#include "../memory/interfaces.h"

int math::time_to_ticks(float time) noexcept
{
    return static_cast<int>((time + 0.5f) / interfaces::globals->interval_per_tick);
}

float math::ticks_to_time(int ticks) noexcept
{
    return interfaces::globals->interval_per_tick * ticks;
}

vec3 math::lerp_vector(const vec3& a, const vec3& b, float fraction) noexcept
{
    return vec3{
        std::lerp(a.x, b.x, fraction),
        std::lerp(a.y, b.y, fraction),
        std::lerp(a.z, b.z, fraction)
    };
}

angle math::calculate_angle(const vec3& src, const vec3& dest, const angle& view) noexcept
{
    const vec3 delta = src - dest;
    angle res{
        rad_to_deg(::math::atan(delta.z / std::hypotf(delta.x, delta.y))) - view.x,
        rad_to_deg(::math::atan(delta.y / delta.x)) - view.y,
        0.0f
    };

    if (delta.x >= 0.0f)
        res.y += 180.0f;

    res.clamp();
    res.normalize();
    return res;
}

void math::angle_to_vectors(const angle& src, vec3* forward, vec3* right, vec3* up) noexcept
{
    vec3 sin{ }, cos{ };
#ifdef __clang__
    sin = { sind(src.x), sind(src.y), sind(src.z) };
    cos = { cosd(src.x), cosd(src.y), cosd(src.z) };
#else
    __m128 cos_res{ };
    const auto sin_res = _mm_sincos_ps(&cos_res,
                                       _mm_set_ps(deg_to_rad(src.x),
                                                  deg_to_rad(src.y),
                                                  deg_to_rad(src.z), 0)).m128_f32;
    sin = { sin_res[3], sin_res[2], sin_res[1] };
    cos = { cos_res.m128_f32[3], cos_res.m128_f32[2], cos_res.m128_f32[1] };
#endif

    if (forward) {
        forward->x = cos.x * cos.y;
        forward->y = cos.x * sin.y;
        forward->z = -sin.x;
    }
    if (right) {
        right->x = -sin.z * sin.x * cos.y + -cos.z * -sin.y;
        right->y = -sin.z * sin.x * sin.y + -cos.z * cos.y;
        right->z = -sin.z * cos.x;
    }
    if (up) {
        up->x = cos.z * sin.x * cos.y + -sin.z * -sin.y;
        up->y = cos.z * sin.x * sin.y + -sin.z * cos.y;
        up->z = cos.z * cos.x;
    }
}

bool math::world_to_screen(const vec3& world, vec2& screen) noexcept
{
    const auto& matrix = interfaces::engine->world_to_screen_matrix();
    const float width = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];

    if (width < 0.001f)
        return false;

    const float inverse = 1.0f / width;
    const d2 screen_size = interfaces::engine->get_screen_size();
    screen = { screen_size.x / 2.0f, screen_size.y / 2.0f };
    screen.x *= 1.0f + (matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3]) * inverse;
    screen.y *= 1.0f - (matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3]) * inverse;

    return true;
}
