#pragma once

#include <cassert>
#include <cmath>
#include "enginemath/vec3.hpp"

namespace enginemath {

struct Vec4 {
    union {
        struct 
        {
            float x;
            float y;
            float z;
            float w;
        };

        float elements[4]; //Allows Vec4 to be accessed by name and index
    };

    //Constructors
    constexpr Vec4() noexcept : x(0.0f), y(0.0f), z(0.0), w(0.0) {}
    constexpr Vec4(float _x, float _y, float _z, float _w) noexcept : x(_x), y(_y), z(_z), w(_w) {}
    explicit constexpr Vec4(float scalar) noexcept : x(scalar), y(scalar), z(scalar), w(scalar) {}

    // "Array like" behaviour functions
    [[nodiscard]] constexpr float operator[](size_t index) const { assert(index < 4); return elements[index]; }
    [[nodiscard]] constexpr float& operator[](size_t index) { assert(index < 4); return elements[index]; } 

    //Simple Operations
    constexpr Vec4 operator+(const Vec4& other) const noexcept { return {x + other.x, y + other.y, z + other.z, w + other.w}; }
    constexpr Vec4 operator-(const Vec4& other) const noexcept { return {x - other.x, y - other.y, z - other.z, w - other.w}; }

    constexpr Vec4 operator*(float scalar) const noexcept { return {x * scalar, y * scalar, z * scalar, w * scalar}; }
    friend inline constexpr Vec4 operator*(const float left, const Vec4& right) {return right * left;} // lets float * Vec4 be possible

    Vec4 operator/(float scalar) const { assert(scalar != 0.0f); return {x / scalar, y / scalar, z / scalar, w / scalar}; }

    constexpr Vec4 operator-() const noexcept { return {-x, -y, -z, -w}; }

    Vec4& operator+=(const Vec4& other) noexcept { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
    Vec4& operator-=(const Vec4& other) noexcept { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
    Vec4& operator*=(float scalar) noexcept { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
    Vec4& operator/=(float scalar) { assert(scalar != 0.0f); x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

    //Comparison
    constexpr bool operator==(const Vec4& other) const noexcept { return (x == other.x && y == other.y && z == other.z && w == other.w); }
    constexpr bool operator!=(const Vec4& other) const noexcept { return (x!= other.x || y != other.y || z != other.z || w != other.w); }

    //Convert to/from Vec3

    static Vec4 toVec4Pos(const Vec3& vector) {return Vec4(vector.x, vector.y, vector.z, 1.0f); }
    static Vec4 toVec4Dir(const Vec3& vector) {return Vec4(vector.x, vector.y, vector.z, 0.0f); }

    static Vec3 toVec3(const Vec4& vector) {return Vec3(vector.x, vector.y, vector.z); }

    const float* data() const noexcept { return &(*this).x; }

    };



} // namespace enginemath

