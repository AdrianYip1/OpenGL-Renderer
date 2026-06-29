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
#include "glp/profiler.h"

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

    std::cout << "GL_RENDERER: " << reinterpret_cast<const char*>(glGetString(GL_RENDERER)) << '\n';
    std::cout << "GL_VENDOR: " << reinterpret_cast<const char*>(glGetString(GL_VENDOR)) << '\n';

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Declaring local variables to pass into ShaderParams
    float dirLight[3] = {1.0f, -1.0f, -1.0f};
    float specularColor[3] = {1.0f, 1.0f, 1.0f};
    float ambientTint[3] = {1.0f, 1.0f, 1.0f};
    float shadowTint[3] = {150.0f / 255.0f, 133.0f / 255.0f, 185.0f / 255.0f};
    float lightTint[3] = {1.0f, 1.0f, 1.0f};
    float rimColor[4] = {0.1f, 0.0f, 0.0f, 1.0f};
    float glossiness = 64.0f;
    float rimThreshold = 0.1f;
    float rimAmount = 0.2f;
    float outlineWidth = 0.0f;
    float gamma = 2.2f;
    float exposure = 0.6f;
    bool bSpecularMap = false;
    bool bRimColor = false;
    bool bOutline = false;
    bool bHDR = false;
    bool bBloom = false;
    int blurPasses = 0;
    float saturation = 1.0f;

    static const char* hdrOptions[] = {"Exposure", "GT", "ACES"};
    int currentHDR = 0;

    static const ModelType modelOptions[] = {GENSHIN, SAMUS, TANGROWTH};
    static const char* modelLabels[] = {"GENSHIN", "SAMUS", "TANGROWTH"};
    int currentModel = 2;

    static const Style shaderOptions[] = {NONE_STYLE, CEL, CARTOON, HATCHING, ISOPHOTES};
    static const char* shaderLabels[] = {"NONE", "Cel (Anime)", "CARTOON", "Hatching (Manga)", "ISOPHOTES"};
    int currentShader = 0;

    Render renderer;
    ModelSelect model;
    Profiler profiler;
    // Render Loop
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = (float) glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        profiler.beginFrame();

        // Connect GUI with params
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shader Controls");

        if (ImGui::BeginCombo("Select Model Option", modelLabels[currentModel])) {
            for (int i = 0; i < IM_ARRAYSIZE(modelOptions); i++) {
                // Check if specific item is currently selected
                bool is_selected_model = (currentModel == i);

                // Render selected hdr
                if (ImGui::Selectable(modelLabels[i], is_selected_model)) {
                    currentModel = i;
                }

                if (is_selected_model) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Select Shader Style", shaderLabels[currentShader])) {
            for (int i = 0; i < IM_ARRAYSIZE(shaderOptions); i++) {
                // Check if specific item is currently selected
                bool is_selected_shader = (currentShader == i);

                // Render selected hdr
                if (ImGui::Selectable(shaderLabels[i], is_selected_shader)) {
                    currentShader = i;
                }

                if (is_selected_shader) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        Style style = shaderOptions[currentShader];
        
        if (style != NONE_STYLE){
            ImGui::SliderFloat3("Directional Light", dirLight, -1.0f, 1.0f);
        }
        if (style == CEL || style == CARTOON) {
            ImGui::ColorEdit3("Specular Color", specularColor);
            ImGui::SliderFloat("Glossiness", &glossiness, 1.0f, 128.0f);
            ImGui::ColorEdit3("Ambient Tint", ambientTint);
            ImGui::ColorEdit3("Light Tint", lightTint);
        }
        if (style == CEL) {
            ImGui::ColorEdit3("Shadow Tint", shadowTint);
            ImGui::SliderFloat("Saturation", &saturation, 0.0f, 3.0f);
        }
        if (style == CARTOON) {
            ImGui::ColorEdit3("Rim Color", rimColor);
        } else if (style == CEL) {
            ImGui::Checkbox("Enable Rim Color", &bRimColor);
                if (bRimColor) {                       // nest: hide sub-params when off
                ImGui::ColorEdit3("Rim Color", rimColor);
                ImGui::SliderFloat("Rim Threshold", &rimThreshold, 0.0f, 1.0f);
                ImGui::SliderFloat("Rim Amount", &rimAmount, 0.0f, 1.0f);
                }
        }
        if (style == CEL) {
            ImGui::Checkbox("Enable Outlines", &bOutline);
            if (bOutline) {
                ImGui::SliderFloat("Outline Width", &outlineWidth, 0.0f, 0.003f);
            }
        }
        if (style == CEL) {
            ImGui::Checkbox("Enable Bloom", &bBloom);
            if (bBloom) {
                ImGui::SliderInt("Blur Passes", &blurPasses, 0, 10);
            }
            ImGui::Checkbox("Enable HDR", &bHDR);
            if (bHDR) {
                ImGui::SliderFloat("Exposure", &exposure, 0.0f, 2.0f);

                if (ImGui::BeginCombo("Select HDR Option", hdrOptions[currentHDR])) {
                    for (int i = 0; i < IM_ARRAYSIZE(hdrOptions); i++) {
                        // Check if specific item is currently selected
                        bool is_selected_hdr = (currentHDR == i);

                        // Render selected hdr
                        if (ImGui::Selectable(hdrOptions[i], is_selected_hdr)) {
                            currentHDR = i;
                        }

                        if (is_selected_hdr) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        }

        ImGui::End();

        ShaderParams params;
        memcpy(params.dirLight, dirLight, sizeof(dirLight));

        float tempSpec[4] = {specularColor[0], specularColor[1], specularColor[2], 1.0f};
        memcpy(params.specularColor, tempSpec , sizeof(params.specularColor));
        params.glossiness = glossiness;
    
        memcpy(params.ambientTint, ambientTint, sizeof(ambientTint));
        memcpy(params.shadowTint, shadowTint, sizeof(shadowTint));
        memcpy(params.lightTint, lightTint, sizeof(lightTint));

        params.saturation = saturation;

        params.bRimColor = bRimColor;
        float tempRim[4] = {rimColor[0], rimColor[1], rimColor[2], 1.0f};
        memcpy(params.rimColor, tempRim, sizeof(params.rimColor));
        params.rimThreshold = rimThreshold;
        params.rimAmount = rimAmount;

        params.bOutline = bOutline;
        params.outlineWidth = outlineWidth;

        params.gamma = gamma;

        params.bHDR = bHDR;
        params.exposure = exposure;
        params.HDRtype = currentHDR;

        params.bBloom = bBloom;
        params.blurPasses = blurPasses;

        enginemath::Mat4 projection = enginemath::Mat4::projectionM(
            enginemath::toRad(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f
        );

        enginemath::Mat4 view = enginemath::Mat4::lookAtM(
            cameraPos, cameraPos + cameraFront, cameraUp
        );
        // Render the model
        model.selectModel(modelOptions[currentModel], shaderOptions[currentShader], params, projection, view, cameraPos, profiler);


        profiler.endFrame();

        // Profiler overlay
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            const float pad = 10.0f;
            ImVec2 pos(viewport->WorkPos.x + viewport->WorkSize.x - pad,
                       viewport->WorkPos.y + viewport->WorkSize.y - pad);
            ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
            ImGui::SetNextWindowBgAlpha(0.5f);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                                     ImGuiWindowFlags_NoNav;

            if (ImGui::Begin("GPU Profiler", nullptr, flags)) {
                double totalMs = 0.0;
                const auto& results = profiler.results();
                if (results.empty()) {
                    ImGui::TextDisabled("No timing data");
                } else {
                    for (const auto& r : results) {
                        if (r.depth > 0) ImGui::Indent(r.depth * 12.0f);
                        ImGui::Text("%s: %.3f ms", r.name.c_str(), r.ms);
                        if (r.depth > 0) ImGui::Unindent(r.depth * 12.0f);
                        if (r.depth == 0) totalMs += r.ms;
                    }
                    ImGui::Separator();
                    ImGui::Text("Total: %.3f ms", totalMs);
                    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
                }
            }
            ImGui::End();
        }

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        renderer.endFrame(window);
    }
    glfwTerminate();
    return 0;
}

