#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mesh/mesh.hpp>
#include <enginemath/mat4.hpp>
#include <shader/shader.hpp>


class Render {
    public:
        void draw(const Mesh& mesh);

        void setPosition(const Shader& shader, const enginemath::Mat4& projectionM, 
                        const enginemath::Mat4& viewM, const enginemath::Mat4& modelM);

        void endFrame(GLFWwindow* window);
        // being and end frame
        //begin frame sets up stuff like uniforms, matrices
        //end frame handles buffers, polls
};

#endif