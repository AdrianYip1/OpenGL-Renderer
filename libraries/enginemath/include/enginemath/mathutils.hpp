#pragma once
#include <cmath>

namespace enginemath {
    constexpr float toRad(float degrees) noexcept { return degrees * (M_PI / 180.0f); }
    constexpr float toDeg(float radians) noexcept { return radians * (180.0f / M_PI); }
}