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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

enginemath::Vec3 cameraPos(0.0f, 0.0f, 3.0f);
enginemath::Vec3 cameraFront(0.0f, 0.0f, -1.0f);
enginemath::Vec3 cameraUp(0.0f, 1.0f, 0.0f);
enginemath::Vec3 cameraAngles(0.0f, -90.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float speed = 2.5f * deltaTime;
    const float rotSpeed = 30.0f * deltaTime;

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
}

Vertex cubeVertices[] = {
    // Back face
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, {}, { 0.0f, 0.0f } },
    // Front face
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, {}, { 0.0f, 0.0f } },
    // Left face
    { { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    // Right face
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    // Bottom face
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    // Top face
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, {}, { 0.0f, 1.0f } },
};

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "glsl-shaders", NULL, NULL);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Mesh cube(cubeVertices, 36);
    Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Render renderer;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        enginemath::Mat4 projection = enginemath::Mat4::projectionM(enginemath::toRad(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
        enginemath::Mat4 view = enginemath::Mat4::lookAtM(cameraPos, cameraPos + cameraFront, cameraUp);
        enginemath::Mat4 model = enginemath::Mat4::identity();

        cube.bind();
        shader.use();
        shader.setFloat("time", currentFrame);
        renderer.setPosition(shader, projection, view, model);
        renderer.draw(cube);

        renderer.endFrame(window);
    }

    glfwTerminate();
    return 0;
}