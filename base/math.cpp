#include <intrin.h>

#include "math.h"
#include "../memory/interface.h"

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
    return (a * (1.0f - fraction)) + (b * fraction);
}

angle math::calculate_angle(const vec3& src, const vec3& dest, const angle& view) noexcept
{
    auto delta = src - dest;
    angle res{ };
    res.x = rad_to_deg(atanf(delta.z / hypotf(delta.x, delta.y))) - view.x;
    res.y = rad_to_deg(atanf(delta.y / delta.x)) - view.y;

    if (delta.x >= 0.0f)
        res.y += 180.0f;

    res.normalize();
    res.clamp();
    return res;
}

void math::angle_to_vectors(const angle& src, vec3* forward, vec3* right, vec3* up) noexcept
{
    vec3 sin{ }, cos{ };
#ifdef SSE_ENABLED
    __m128 cos_res{ };
    const auto sin_res = _mm_sincos_ps(&cos_res, _mm_set_ps(deg_to_rad(src.x), deg_to_rad(src.y), deg_to_rad(src.z), 0)).m128_f32;
    sin = { sin_res[3], sin_res[2], sin_res[1] };
    cos = { cos_res.m128_f32[3], cos_res.m128_f32[2], cos_res.m128_f32[1] };
#else
    const auto sin_cos = [](const float rad, float& sin, float& cos) { sin = std::sin(rad); cos = std::cos(rad); };
    sin_cos(deg_to_rad(src.x), sin.x, cos.x);
    sin_cos(deg_to_rad(src.y), sin.y, cos.y);
    sin_cos(deg_to_rad(src.z), sin.z, cos.z);
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

w2s_result math::world_to_screen(const vec3& world) noexcept
{
    w2s_result ret{ };

    const auto& matrix = interfaces::engine->world_to_screen_matrix();
    const float width = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
    d2 screen_size{ };
    interfaces::engine->get_screen_size(&screen_size.x, &screen_size.y);

    if (width > 0.001f)
        ret.successful = true;

    const float inverse = 1.0f / width;
    ret.screen.x = (matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3]) * inverse;
    ret.screen.y = (matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3]) * inverse;
    ret.screen.x = (screen_size.x / 2.0f) + (ret.screen.x * screen_size.x) / 2.0f;
    ret.screen.y = (screen_size.y / 2.0f) - (ret.screen.y * screen_size.y) / 2.0f;

    return ret;
}
