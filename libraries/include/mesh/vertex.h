#pragma once

#include <enginemath/vec3.hpp>
#include <enginemath/vec2.hpp>

struct Vertex {
    enginemath::Vec3 position;
    enginemath::Vec3 normal;
    enginemath::Vec2 texcoords;
};