#ifndef MODELSELECT_H
#define MODELSELECT_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <enginemath/vec3.hpp>
#include <enginemath/mat4.hpp>
#include <enginemath/mathutils.hpp>
#include <shader/shader.hpp>
#include <render/render.hpp>
#include <mesh/mesh.hpp>
#include <mesh/vertex.h>
#include <camera/camera.hpp>
#include <model/model.hpp>

// modelSelect will contain methods for rendering specific models:
// Setting their shaders, textures, positions, and scaling

class ModelSelect {
    public: 
        void selectModel(const char* name) {
            
        }
}

#endif