#pragma once

#include <cassert>
#include <cmath>

namespace enginemath {

struct Vec2 {
    union {
        struct 
        {
            float x;
            float y;
        };

        float data[2]; //Allows Vec2 to be accessed by name and index
    };

    //Constructors
    constexpr Vec2() noexcept : x(0.0f), y(0.0f) {}
    constexpr Vec2(float _x, float _y) noexcept : x(_x), y(_y) {}
    explicit constexpr Vec2(float scalar) noexcept : x(scalar), y(scalar) {}

    // "Array like" behaviour functions
    [[nodiscard]] constexpr float operator[](size_t index) const { assert(index < 2); return data[index]; }
    [[nodiscard]] constexpr float& operator[](size_t index) { assert(index < 2); return data[index]; } 

    //Simple Operations
    constexpr Vec2 operator+(const Vec2& other) const noexcept { return {x + other.x, y + other.y}; }
    constexpr Vec2 operator-(const Vec2& other) const noexcept { return {x - other.x, y - other.y}; }
    constexpr Vec2 operator*(const Vec2& other) const noexcept { return {x * other.x, y * other.y}; }
    constexpr Vec2 operator*(float scalar) const noexcept { return {x * scalar, y * scalar}; }
    friend inline constexpr Vec2 operator*(const float left, const Vec2& right) {return right * left;} // lets float * Vec2 be possible

    Vec2 operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar}; }

    constexpr Vec2 operator-() const noexcept { return {-x, -y}; }

    Vec2& operator+=(const Vec2& other) noexcept { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) noexcept { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; return *this; }

    //Comparison
    constexpr bool operator==(const Vec2& other) const noexcept { return (x == other.x && y == other.y); }
    constexpr bool operator!=(const Vec2& other) const noexcept { return (x!= other.x || y != other.y); }

    [[nodiscard]] bool equal_within_decimals(const Vec2& other, const int num_points) const noexcept {
        assert(num_points >= 0);
        float eps = std::pow(10.0f, -num_points); 
        return std::fabs(x - other.x) < eps && std::fabs(y - other.y) < eps;
    }
    [[nodiscard]] bool equal_within_tolerance(const Vec2& other, const float tolerance) const noexcept {
        float eps = std::fabs(tolerance);
        return std::fabs(x - other.x) < eps && std::fabs(y - other.y) < eps;
    }

    //Getters
    //MagnitudeSq is cheaper than finding full magnitude for comparison
    [[nodiscard]] float magnitudeSq() const noexcept {return x*x + y*y; }
    [[nodiscard]] float magnitude() const noexcept { return std::sqrt(x * x + y * y); }
    [[nodiscard]] constexpr float dot(const Vec2& other) const noexcept { return (x * other.x) + (y * other.y); }
    [[nodiscard]] constexpr float cross(const Vec2& other) const noexcept { return (x * other.y) - (other.x * y); }
    [[nodiscard]] constexpr Vec2 perpCCW() const { return {-y, x}; }
    [[nodiscard]] constexpr Vec2 perpCW() const { return {y, -x}; }

    [[nodiscard]] Vec2 normalized() const { float mag = magnitude(); assert(mag > 0.0f); return *this / mag; }
    void normalize() { *this = normalized(); }

    [[nodiscard]] static float distance(const Vec2& a, const Vec2& b) noexcept { return (b - a).magnitude(); }

    // Projections
    [[nodiscard]] Vec2 projectOnto(const Vec2& w) const { //project self onto direction of w
        assert(w.magnitudeSq() > 0);
        return w * (dot(w) / w.dot(w));
    }

    //Reflection

    [[nodiscard]] Vec2 reflectAcross(const Vec2& lineDir) const {
        Vec2 normal = lineDir.perpCCW().normalized();
        return *this - 2.0f * (*this).projectOnto(normal);
    }

    // Clamping t to [0, 1]
    [[nodiscard]] static Vec2 lerp(const Vec2& a, const Vec2& b, float t) noexcept { assert(t >= 0 && t <= 1); return a + (b - a) * t; }

    //Directions
    [[nodiscard]] constexpr static Vec2 up() noexcept { return Vec2(0, 1); }
    [[nodiscard]] constexpr static Vec2 down() noexcept { return Vec2(0, -1); }
    [[nodiscard]] constexpr static Vec2 left() noexcept { return Vec2(-1, 0); }
    [[nodiscard]] constexpr static Vec2 right() noexcept { return Vec2(1, 0); }

};

} // namespace enginemath

