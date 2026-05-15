#pragma once

#include <cassert>
#include <cmath>

namespace enginemath {

struct Vec3 {
    union {
        struct 
        {
            float x;
            float y;
            float z;
        };

        float elements[3]; //Allows Vec3 to be accessed by name and index
    };

    //Constructors
    constexpr Vec3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vec3(float _x, float _y, float _z) noexcept : x(_x), y(_y), z(_z) {}
    explicit constexpr Vec3(float scalar) noexcept : x(scalar), y(scalar), z(scalar) {}

    // "Array like" behaviour functions
    [[nodiscard]] constexpr float operator[](size_t index) const { assert(index < 3); return elements[index]; }
    [[nodiscard]] constexpr float& operator[](size_t index) { assert(index < 3); return elements[index]; } 

    //Simple Operations
    constexpr Vec3 operator+(const Vec3& other) const noexcept { return {x + other.x, y + other.y, z + other.z}; }
    constexpr Vec3 operator-(const Vec3& other) const noexcept { return {x - other.x, y - other.y, z - other.z}; }
    constexpr Vec3 operator*(const Vec3& other) const noexcept { return {x * other.x, y * other.y, z * other.z}; }
    constexpr Vec3 operator*(float scalar) const noexcept { return {x * scalar, y * scalar, z * scalar}; }
    friend inline constexpr Vec3 operator*(const float left, const Vec3& right) {return right * left;} // lets float * Vec3 be possible

    Vec3 operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar, z / scalar}; }

    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

    Vec3& operator+=(const Vec3& other) noexcept { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-=(const Vec3& other) noexcept { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; z /= scalar; return *this; }

    //Comparison
    constexpr bool operator==(const Vec3& other) const noexcept { return (x == other.x && y == other.y && z == other.z); }
    constexpr bool operator!=(const Vec3& other) const noexcept { return (x!= other.x || y != other.y || z != other.z); }

    [[nodiscard]] bool equal_within_decimals(const Vec3& other, const int num_points) const noexcept {
        assert(num_points >= 0);
        float eps = std::pow(10.0f, -num_points); 
        return std::fabs(x - other.x) < eps && std::fabs(y - other.y) < eps && std::fabs(z - other.z) < eps;
    }
    [[nodiscard]] bool equal_within_tolerance(const Vec3& other, const float tolerance) const noexcept {
        float eps = std::fabs(tolerance);
        return std::fabs(x - other.x) < eps && std::fabs(y - other.y) < eps && std::fabs(z - other.z) < eps;
    }

    //Getters
    //MagnitudeSq is cheaper than finding full magnitude for comparison
    [[nodiscard]] float magnitudeSq() const noexcept {return x * x + y * y + z * z; }
    [[nodiscard]] float magnitude() const noexcept { return std::sqrt(x * x + y * y + z * z); }
    [[nodiscard]] constexpr float dot(const Vec3& other) const noexcept { return (x * other.x) + (y * other.y) + (z * other.z); }
    [[nodiscard]] constexpr Vec3 cross(const Vec3& other) const noexcept { return {(y * other.z - z * other.y), (z * other.x - x * other.z), (x * other.y - y * other.x)}; }

    [[nodiscard]] Vec3 normalized() const { float mag = magnitude(); assert(mag > 0.0f); return *this / mag; }
    void normalize() { *this = normalized(); }

    [[nodiscard]] static float distance(const Vec3& a, const Vec3& b) noexcept { return (b - a).magnitude(); }
    bool basicallyZero(float eps = 1e-6f) const {return (magnitudeSq() <= eps * eps); }

    // Projections
    [[nodiscard]] Vec3 projectOnto(const Vec3& w) const { //project self onto direction of w
        float denom = w.dot(w);
        assert(denom > 0.0f);
        return w * (dot(w) / denom);
    }

    //Reflection

    [[nodiscard]] Vec3 reflectAcross(const Vec3& unitNormal) const { //unitNormal must be normalized
        return *this - 2.0f * dot(unitNormal) * unitNormal; 
    }

    // Clamping t to [0, 1]
    [[nodiscard]] static Vec3 lerp(const Vec3& a, const Vec3& b, float t) noexcept { assert(t >= 0 && t <= 1); return a + (b - a) * t; }

    //Directions
    [[nodiscard]] constexpr static Vec3 right() noexcept { return Vec3(1, 0, 0); } //upX
    [[nodiscard]] constexpr static Vec3 left() noexcept { return Vec3(-1, 0, 0); } //downX

    [[nodiscard]] constexpr static Vec3 up() noexcept { return Vec3(0, 1, 0); } //upY
    [[nodiscard]] constexpr static Vec3 down() noexcept { return Vec3(0, -1, 0); } //downY

    [[nodiscard]] constexpr static Vec3 forward() noexcept { return Vec3(0, 0, 1); } //upZ
    [[nodiscard]] constexpr static Vec3 backward() noexcept { return Vec3(0, 0, -1); } //downZ

    const float* data() const noexcept { return &(*this).x; }
};

} // namespace enginemath

