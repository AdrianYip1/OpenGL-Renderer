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
#include <modelSelect.hpp>

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

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        usingShader = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        usingShader = true;
    }
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "glsl-shaders", NULL, NULL);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Declaring local variables to pass into ShaderParams
    float dirLight[3] = {1.0f, -1.0f, -1.0f};
    float specularColor[3] = {1.0f, 1.0f, 1.0f};
    float ambientTint[3] = {1.0f, 1.0f, 1.0f};
    float shadowTint[3] = {0.0f, 0.0f, 0.0f};
    float lightTint[3] = {1.0f, 1.0f, 1.0f};
    float rimColor[4] = {0.1f, 0.0f, 0.0f, 1.0f};
    float glossiness = 64.0f;
    float rimThreshold = 0.1f;
    float rimAmount = 0.2f;
    float outlineWidth = 0.0f;
    float outlineBurnIntensity = 0.0f;
    float outlineLightInfluence = 0.0f;
    float gamma = 2.2f;
    float exposure = 1.0f;
    bool bSpecularMap = false;
    bool bRimColor = false;
    bool bOutline = false;
    bool bHDR = false;
    bool bBloom = false;
    int blurPasses = 0;

    ModelSelect model;
    // Render Loop
    Render renderer;
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Connect GUI with params
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shader Controls");

        ImGui::SliderFloat3("Directional Light", dirLight, -1.0, 1.0f);

        ImGui::ColorEdit3("Specular Color", specularColor);
        ImGui::SliderFloat("Glossiness", &glossiness, 1.0f, 128.0f);
        
        ImGui::ColorEdit3("Ambient Tint", ambientTint);
        ImGui::ColorEdit3("Shadow Tint", shadowTint);
        ImGui::ColorEdit3("Light Tint", lightTint);

        ImGui::Checkbox("Enable Rim Color", &bRimColor);
        ImGui::ColorEdit3("Rim Color", rimColor);
        ImGui::SliderFloat("Rim Threshold", &rimThreshold, 0.0f, 1.0f);
        ImGui::SliderFloat("Rim Amount", &rimAmount, 0.0f, 1.0f);

        ImGui::Checkbox("Enable Outlines", &bOutline);
        ImGui::SliderFloat("Outline Width", &outlineWidth, 0.0f, 0.003f);
        ImGui::SliderFloat("Burn Intensity", &outlineBurnIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Light Influence", &outlineLightInfluence, 0.0f, 1.0f);

        ImGui::SliderFloat("Gamma", &gamma, 0.0f, 3.0f);

        ImGui::Checkbox("Enable HDR", &bHDR);
        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 5.0f);

        ImGui::Checkbox("Enable Bloom", &bBloom);
        ImGui::SliderInt("Blur Passes", &blurPasses, 0, 10);
        ImGui::End();

        ShaderParams params;
        memcpy(params.dirLight, dirLight, sizeof(dirLight));

        float tempSpec[4] = {specularColor[0], specularColor[1], specularColor[2], 1.0f};
        memcpy(params.specularColor, tempSpec , sizeof(params.specularColor));
        params.glossiness = glossiness;
    
        memcpy(params.ambientTint, ambientTint, sizeof(ambientTint));
        memcpy(params.shadowTint, shadowTint, sizeof(shadowTint));
        memcpy(params.lightTint, lightTint, sizeof(lightTint));

        params.bRimColor = bRimColor;
        float tempRim[4] = {rimColor[0], rimColor[1], rimColor[2], 1.0f};
        memcpy(params.rimColor, tempRim, sizeof(params.rimColor));
        params.rimThreshold = rimThreshold;
        params.rimAmount = rimAmount;

        params.bOutline = bOutline;
        params.outlineWidth = outlineWidth;
        params.outlineBurnIntensity = outlineBurnIntensity;
        params.outlineLightInfluence = outlineLightInfluence;

        params.gamma = gamma;

        params.bHDR = bHDR;
        params.exposure = exposure;

        params.bBloom = bBloom;
        params.blurPasses = blurPasses;

        enginemath::Mat4 projection = enginemath::Mat4::projectionM(
            45.0f * M_PI / 180.0f,
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );

        enginemath::Mat4 view = enginemath::Mat4::lookAtM(
            cameraPos, cameraPos + cameraFront, cameraUp
        );
        // Render the model
        model.selectModel(GENSHIN, ANIME, params, projection, view, cameraPos);





        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        renderer.endFrame(window);
    }
    glfwTerminate();
    return 0;
}

