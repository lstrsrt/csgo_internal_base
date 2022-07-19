#pragma once

#include <numbers>
#include <random>

#include "base.h"

struct w2s_result {
    vec2 screen{ };
    bool successful{ };
};

namespace math {

    constexpr float rad_to_deg(float rad) noexcept
    {
        return rad * (180.0f / std::numbers::pi_v<float>);
    }
    
    constexpr float deg_to_rad(float deg) noexcept
    {
        return deg * (std::numbers::pi_v<float> / 180.0f);
    }
    
    int time_to_ticks(float time) noexcept;
    float ticks_to_time(int ticks) noexcept;
    vec3 lerp_vector(const vec3& a, const vec3& b, float fraction) noexcept; // Use std::lerp to interpolate floats
    angle calculate_angle(const vec3& src, const vec3& dest, const angle& view) noexcept;
    void angle_to_vectors(const angle& src, vec3* forward = nullptr, vec3* right = nullptr, vec3* up = nullptr) noexcept;
    w2s_result world_to_screen(const vec3& world) noexcept;

    namespace random {

        inline std::mt19937 engine{ std::random_device{ }() };

        template<class ty> requires std::_Is_any_of_v<ty, short, int, long, long long, unsigned short, unsigned int,
            unsigned long, unsigned long long>
        inline ty integral(ty min, ty max) noexcept
        {
            return std::uniform_int_distribution{ min, max }(engine);
        }

        template<class ty> requires std::is_floating_point_v<ty>
        inline ty floating_point(ty min, ty max) noexcept
        {
            return std::uniform_real_distribution{ min, max }(engine);
        }

        template<class ty> requires std::is_enum_v<ty>
        inline ty enumerator(ty min, ty max) noexcept
        {
            return static_cast<ty>(integral(to_underlying(min), to_underlying(max)));
        }

    }

}
