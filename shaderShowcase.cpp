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
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stylesAndModels.hpp>

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

enginemath::Vec3 cameraPos(0.0f, 0.0f, 3.0f);
enginemath::Vec3 cameraFront(0.0f, 0.0f, -1.0f);
enginemath::Vec3 cameraUp(0.0f, 1.0f, 0.0f);
enginemath::Vec3 cameraAngles(0.0f, -90.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float usingShader = true;

Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Shader& shader) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 4.5f * deltaTime;
    const float rotSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.directionalInput(cameraPos, cameraFront, speed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.directionalInput(cameraPos, -cameraFront, speed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.directionalInput(cameraPos, -cameraFront.cross(cameraUp).normalized(), speed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.directionalInput(cameraPos, cameraFront.cross(cameraUp).normalized(), speed);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) cameraAngles.x += rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) cameraAngles.x -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) cameraAngles.y -= rotSpeed;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) cameraAngles.y += rotSpeed;

    camera.angularInput(cameraFront, cameraAngles);

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        shader = Shader("shaders/shader.vert", "shaders/shader.frag");
        usingShader = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        shader = Shader("shaders/Stylized Shaders/anime/anime.vert", "shaders/Stylized Shaders/anime/anime.frag");
        usingShader = true;
    }
}

