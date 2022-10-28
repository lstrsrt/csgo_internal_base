#pragma once

#include <intrin.h>
#include <numbers>
#include <random>

#include "base.h"

struct w2s_result {
    vec2 screen{ };
    bool successful{ };
};

namespace math {

    static constexpr auto pi = std::numbers::pi_v<float>;

    constexpr float rad_to_deg(float rad) noexcept
    {
        return rad * (180.0f / pi);
    }

    constexpr float deg_to_rad(float deg) noexcept
    {
        return deg * (pi / 180.0f);
    }

    inline float sin(float x) noexcept
    {
    #ifdef __clang__
        return __builtin_sinf(x);
    #else
        return _mm_cvtss_f32(_mm_sin_ps(_mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    inline float cos(float x) noexcept
    {
    #ifdef __clang__
        return __builtin_cosf(x);
    #else
        return _mm_cvtss_f32(_mm_cos_ps(_mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    inline float sind(float x) noexcept
    {
        return sin(deg_to_rad(x));
    }

    inline float cosd(float x) noexcept
    {
        return cos(deg_to_rad(x));
    }

    inline float asin(float x) noexcept
    {
    #ifdef __clang__
        return __builtin_asinf(x);
    #else
        return _mm_cvtss_f32(_mm_asin_ps(_mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    inline float acos(float x) noexcept
    {
    #ifdef __clang__
        return __builtin_acosf(x);
    #else
        return _mm_cvtss_f32(_mm_acos_ps(_mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    inline float atan(float x) noexcept
    {
    #ifdef __clang__
        return __builtin_atanf(x);
    #else
        return _mm_cvtss_f32(_mm_atan_ps(_mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    inline float atan2(float y, float x) noexcept
    {
    #ifdef __clang__
        return __builtin_atan2f(y, x);
    #else
        return _mm_cvtss_f32(_mm_atan2_ps(_mm_set_ps(0.f, 0.f, 0.f, y),
                                          _mm_set_ps(0.f, 0.f, 0.f, x)));
    #endif
    }

    int time_to_ticks(float time) noexcept;
    float ticks_to_time(int ticks) noexcept;
    vec3 lerp_vector(const vec3& a, const vec3& b, float fraction) noexcept;
    angle calculate_angle(const vec3& src, const vec3& dest, const angle& view) noexcept;
    void angle_to_vectors(const angle& src, vec3* forward = nullptr, vec3* right = nullptr,
                          vec3* up = nullptr) noexcept;
    w2s_result world_to_screen(const vec3& world) noexcept;

    namespace random {

        inline std::mt19937 engine{ std::random_device{ }() };

        template<std::integral ix>
        inline ix get(ix min, ix max) noexcept
        {
            return std::uniform_int_distribution{ min, max }(engine);
        }

        template<std::floating_point fp>
        inline fp get(fp min, fp max) noexcept
        {
            return std::uniform_real_distribution{ min, max }(engine);
        }

        template<enumerator en>
        inline en get(en min, en max) noexcept
        {
            return static_cast<en>(get(util::to_underlying(min), util::to_underlying(max)));
        }

    }

}
