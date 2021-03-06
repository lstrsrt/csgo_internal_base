#pragma once

#include <limits>

struct angle {
    float x{ }, y{ }, z{ };

    constexpr angle() noexcept = default;
    constexpr angle(float x, float y, float z) noexcept
        : x(x), y(y), z(z) { }

    bool operator==(const angle& rhs) const noexcept
    {
        return (fabsf(x - rhs.x) <= std::numeric_limits<float>::epsilon() &&
            fabsf(y - rhs.y) <= std::numeric_limits<float>::epsilon() &&
            fabsf(z - rhs.z) <= std::numeric_limits<float>::epsilon());
    }

    bool operator!=(const angle& rhs) const noexcept
    {
        return (fabsf(x - rhs.x) > std::numeric_limits<float>::epsilon() ||
            fabsf(y - rhs.y) > std::numeric_limits<float>::epsilon() ||
            fabsf(z - rhs.z) > std::numeric_limits<float>::epsilon());
    }

    angle operator+(const angle& rhs) const
    {
        return angle(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    angle operator-(const angle& rhs) const
    {
        return angle(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    angle operator*(const angle& rhs) const
    {
        return angle(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    angle operator/(const angle& rhs) const
    {
        return angle(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    constexpr angle& operator=(const angle& rhs) noexcept
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    constexpr angle& operator+=(const angle& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr angle& operator-=(const angle& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr angle& operator*=(const angle& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    constexpr angle& operator/=(const angle& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    constexpr angle& operator+=(const float rhs) noexcept
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    constexpr angle& operator-=(const float rhs) noexcept
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    constexpr angle& operator*=(const float rhs) noexcept
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    constexpr angle& operator/=(const float rhs) noexcept
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    constexpr void clamp() noexcept
    {
        x = std::clamp(x, -89.0f, 89.0f);
        y = std::clamp(y, -180.0f, 180.0f);
        z = 0.0f;
    }
    
    constexpr auto length_sqr() const noexcept
    {
        return (x * x + y * y + z * z);
    }

    constexpr auto length2d_sqr() const noexcept
    {
        return (x * x + y * y);
    }

    auto length() const noexcept
    {
        return sqrtf(length_sqr());
    }

    auto length2d() const noexcept
    {
        return sqrtf(length2d_sqr());
    }

    void normalize() noexcept
    {
        *this /= length();
    }
};
