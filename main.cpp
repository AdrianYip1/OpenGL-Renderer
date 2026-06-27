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
        shader = Shader("shaders/Stylized Shaders/cel/cel.vert", "shaders/Stylized Shaders/cel/cel.frag");
        usingShader = true;
    }
}

void shaderWithoutOutline( Shader& mainShader,  Model& model) {
    mainShader.use();
    glCullFace(GL_BACK);
    model.Draw(mainShader);
}

void shaderWithOutline( Shader& outlineShader,  Model& model) {
    outlineShader.use();
    glCullFace(GL_FRONT);
    model.Draw(outlineShader);

    glCullFace(GL_BACK);
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
    //Shader shader("shaders/shader.vert", "shaders/shader.frag");
    Shader shader("shaders/Stylized Shaders/cel/cel.vert", "shaders/Stylized Shaders/cel/cel.frag");
    Shader outlineShader("shaders/Stylized Shaders/cel/outline.vert", "shaders/Stylized Shaders/cel/outline.frag");
        
    //sphere vertices and indices
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    int stacks = 128, slices = 128;

    for(int i = 0; i <= stacks; i++) {
        float phi = M_PI * i / stacks;
        for(int j = 0; j <= slices; j++) {
            float theta = 2 * M_PI * j / slices;
            
            Vertex v;
            v.position = { sin(phi)*cos(theta), cos(phi), sin(phi)*sin(theta) };
            v.normal = v.position; // unit sphere: normal == position
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
    Model genshin("models/Genshin Impact Alice/Genshin Impact Alice by Animanpower.fbx");
    Render renderer;

    float lightDir[3] = { 1.0f, -1.0f, -1.0f };
    float glossiness = 64;
    float rimColor[3] = { 0.1f, 0.0f, 0.0f };

    unsigned int hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    unsigned int colourBuffers[2];
    glGenTextures(2, colourBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colourBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, colourBuffers[i], 0);
    }
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Shader blurShader("shaders/blur.vert", "shaders/blur.frag");
    Shader hdrShader("shaders/hdr.vert", "shaders/hdr.frag");
    renderer.setupQuad();
    float exposure = 0.6f;


    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, shader);



        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Shader Controls");
        ImGui::SliderFloat3("Directional Light", lightDir, -1.0, 1.0f);
        ImGui::SliderFloat("Glossiness", &glossiness, 1.0f, 128.0f);
        ImGui::SliderFloat("Exposure", &exposure, 0.0f, 2.0f);
        ImGui::ColorEdit3("Rim Color", rimColor);
        ImGui::End();

        // Pass 1
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
        shader.setVec3("uDirectionalLight", enginemath::Vec3(lightDir[0], lightDir[1], lightDir[2]));

        shader.setVec4("uSpecular", enginemath::Vec4(1.0, 1.0, 1.0, 0.0));
        shader.setVec4("uRimColor", enginemath::Vec4(rimColor[0], rimColor[1], rimColor[2], 1.0));
        shader.setFloat("uGlossiness", glossiness);

        shader.setMat4("model", enginemath::Mat4::identity());

        // * enginemath::Mat4::scaleM(0.05f, 0.05f, 0.05f)
        enginemath::Mat4 genshinModel = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f);
        shader.setMat4("model", genshinModel);

        shaderWithoutOutline(shader, genshin);

        if (usingShader) {
            outlineShader.use();
            outlineShader.setMat4("projection", projection);
            outlineShader.setMat4("view", view);
            outlineShader.setMat4("model", genshinModel);
            outlineShader.setFloat("uOutlineWidth", 0.001);
            shaderWithOutline(outlineShader, genshin);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Pass 2
        blurShader.use();
        blurShader.setInt("image", 0);
        bool horizontal = false;
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            blurShader.setBool("isHorizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, i == 0 ? colourBuffers[1] : pingpongBuffer[!horizontal]);
            renderer.drawQuad();
            horizontal = !horizontal;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Pass 3
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colourBuffers[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);

        hdrShader.setInt("hdrBuffer", 0);
        hdrShader.setInt("bloomBlur", 1);
        hdrShader.setFloat("exposure", exposure);
        renderer.drawQuad();

        
        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        renderer.endFrame(window);
    }

    glfwTerminate();
    return 0;
}


