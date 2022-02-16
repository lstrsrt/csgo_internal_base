#pragma once

struct d2 {
    int x{ }, y{ };

    constexpr d2& operator+=(const d2& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr d2& operator-=(const d2& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr d2& operator*=(const d2& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    constexpr d2& operator/=(const d2& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    constexpr d2& operator+=(const int rhs) noexcept
    {
        x += rhs;
        y += rhs;
        return *this;
    }

    constexpr d2& operator-=(const int rhs) noexcept
    {
        x -= rhs;
        y -= rhs;
        return *this;
    }

    constexpr d2& operator*=(const int rhs) noexcept
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    constexpr d2& operator/=(const int rhs) noexcept
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }

    d2 operator+(const int rhs) const noexcept
    {
        return d2(x + rhs, y + rhs);
    }
	
	d2 operator-(const int rhs) const noexcept
    {
        return d2(x - rhs, y - rhs);
    }
	
    d2 operator*(const int rhs) const noexcept
    {
        return d2(x * rhs, y * rhs);
    }
	
	d2 operator/(const int rhs) const noexcept
    {
        return d2(x / rhs, y / rhs);
    }

    d2 operator+(const d2& rhs) const noexcept
    {
        return d2(x + rhs.x, y + rhs.y);
    }

    d2 operator-(const d2& rhs) const noexcept
    {
        return d2(x - rhs.x, y - rhs.y);
    }
	
    d2 operator*(const d2& rhs) const noexcept
    {
        return d2(x * rhs.x, y * rhs.y);
    }
	
	d2 operator/(const d2& rhs) const noexcept
    {
        return d2(x / rhs.x, y / rhs.y);
    }
};
