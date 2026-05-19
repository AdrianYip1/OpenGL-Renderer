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
}



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
    //Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Shader shader("shaders/Stylized Shaders/anime/anime.vert", "shaders/Stylized Shaders/anime/anime.frag");
    
        
    //sphere vertices and indices
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int stacks = 128, slices = 128;

    for(int i = 0; i <= stacks; i++) {
        float phi = M_PI * i / stacks;
        for(int j = 0; j <= slices; j++) {
            float theta = 2 * M_PI * j / slices;
            
            Vertex v;
            v.position  = { sin(phi)*cos(theta), cos(phi), sin(phi)*sin(theta) };
            v.normal    = v.position; // unit sphere: normal == position
            v.texcoords = { (float)j/slices, (float)i/stacks };
            vertices.push_back(v);
        }
    }

    for(int i = 0; i < stacks; i++) {
        for(int j = 0; j < slices; j++) {
            int top = i * (slices + 1) + j;
            int bottom = top + slices + 1;
            
            indices.push_back(top);
            indices.push_back(bottom);
            indices.push_back(top + 1);
            
            indices.push_back(top + 1);
            indices.push_back(bottom);
            indices.push_back(bottom + 1);
        }
    }

    Mesh sphere(vertices, indices, {});
    Model miku("models/miku/miku.obj");
    Render renderer;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //testShader.use();
        //testShader.setFloat("iTime", (float)glfwGetTime());
        //testShader.setVec2("iResolution", enginemath::Vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));
        //renderer.render2DShader(testShader);

        shader.use();

        enginemath::Mat4 projection = enginemath::Mat4::projectionM(
            45.0f * M_PI / 180.0f,
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );
        enginemath::Mat4 view = enginemath::Mat4::lookAtM(
            cameraPos, cameraPos + cameraFront, cameraUp
        );
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("uCameraPos", cameraPos);
        shader.setVec3("uDirectionalLight", enginemath::Vec3(1.0, -1.0, -1.0));

        //shader.setVec4("uAmbient", enginemath::Vec4(0.4, 0.4, 0.4, 1.0));
        //shader.setVec4("uDirectional", enginemath::Vec4(0.0, 0.9, 0.9, 1.0));
        shader.setVec4("uSpecular", enginemath::Vec4(1.0, 1.0, 1.0, 0.0));
        shader.setVec4("uRimColor", enginemath::Vec4(0.05, 0.05, 0.05, 1.0));
        shader.setFloat("uGlossiness", 16);

        shader.setMat4("model", enginemath::Mat4::identity());
        //renderer.draw(sphere);

        // scale from MMD units (~18 tall) to ~1.8 world units, center vertically
        enginemath::Mat4 mikuModel = enginemath::Mat4::rotateY(enginemath::toRad(180)) * enginemath::Mat4::translationM(0.0f, -0.9f, 0.0f) * enginemath::Mat4::scaleM(0.1f, 0.1f, 0.1f);
        shader.setMat4("model", mikuModel);
        miku.Draw(shader);



        renderer.endFrame(window);
    }

    glfwTerminate();
    return 0;
}