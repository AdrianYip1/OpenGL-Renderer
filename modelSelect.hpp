#ifndef MODELSELECT_H
#define MODELSELECT_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <enginemath/vec3.hpp>
#include <enginemath/vec4.hpp>
#include <enginemath/mat4.hpp>
#include <enginemath/mathutils.hpp>
#include <shader/shader.hpp>
#include <render/render.hpp>
#include <mesh/mesh.hpp>
#include <mesh/vertex.h>
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <stylesAndModels.hpp>



// modelSelect will contain methods for rendering specific models:
// Setting their shaders, textures, positions, and scaling

//ShaderParams will consist of the uniform information needed and boolian values for
// different effects such as outlining, bloom, hdr,...

class ModelSelect {
    public:

        // Constructor that sets all the points to null
        ModelSelect(Render renderer) {
            this->renderer = new Render;
            this->renderer->setupQuad();

            currentlyLoaded = nullptr;
            currentModel = NONE_MODEL;

            currentShader = nullptr;
            outlineShader = nullptr;
            currentStyle = NONE_STYLE;

            passthroughShader = new Shader("shaders/Stylized Shaders/passthrough.vert", 
                                          "shaders/Stylized Shaders/passthrough.frag");

            gBufferShader = new Shader("shaders/Stylized Shaders/anime/anime.geo.vert",
                                       "shaders/Stylized Shaders/anime/anime.geo.frag");

            bloomShader = new Shader("shaders/blur.vert", 
                                     "shaders/blur.frag");

            hdrShader = new Shader("shaders/hdr.vert", 
                                   "shaders/hdr.frag");

            // Set up final pass FBO + texture
            glGenFramebuffers(1, &finalPassFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, finalPassFBO);

            glGenTextures(1, &finalPassTexture);
            glBindTexture(GL_TEXTURE_2D, finalPassTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, finalPassTexture, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Set up gBuffer
            glGenFramebuffers(1, &gBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

            // Set up position
            glGenTextures(1, &gPosition);
            glBindTexture(GL_TEXTURE_2D, gPosition);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

            // Set up normal
            glGenTextures(1, &gNormal);
            glBindTexture(GL_TEXTURE_2D, gNormal);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

            // Set up albedo
            glGenTextures(1, &gAlbedoSpec);
            glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

            unsigned int textureAttachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
            glDrawBuffers(3, textureAttachments);

            // Renderbuffer object for depth
            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Setup Lighting FBO and color buffers
            glGenFramebuffers(1, &lightingFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);

            // Set up colour attachments
            glGenTextures(2, lightingColorTextures);
            for (int i = 0; i < 2; i++) {
                glBindTexture(GL_TEXTURE_2D, lightingColorTextures[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0 , GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, lightingColorTextures[i], 0);
            }
            
            unsigned int lightAttachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, lightAttachments);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Set up bloom
            glGenFramebuffers(2, bloomFBO);
            for (int i = 0; i < 2; i++) {
                glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[i]);

                glGenTextures(1, &bloomTexture[i]);
                glBindTexture(GL_TEXTURE_2D, bloomTexture[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTexture[i], 0);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Set up HDR
            glGenFramebuffers(1, &hdrFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdrTexture, 0);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void selectModel(ModelType model, Style shaderStyle, ShaderParams& params, enginemath::Mat4 projection,
        enginemath::Mat4 view, enginemath::Vec3 cameraPos) {

            switch (model) {
                case (GENSHIN): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/genshin/genshin.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 genshinModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f);
                    selectStyle(shaderStyle, params, projection, view, cameraPos, genshinModelMat);
                    break;
                }
                case (SAMUS): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/samus/samus.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 samusModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f) * enginemath::Mat4::scaleM(0.1f, 0.1f, 0.1f);
                    selectStyle(shaderStyle, params, projection, view, cameraPos, samusModelMat);
                    break;
                }
                case (TANGROWTH): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/tangrowth/tangrowth.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 tanModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f) * enginemath::Mat4::scaleM(0.1f, 0.1f, 0.1f);
                    selectStyle(shaderStyle, params, projection, view, cameraPos, tanModelMat);
                    break;
                }

                default: break;
            };
        }


    private:
        Render* renderer;
        Model* currentlyLoaded;
        ModelType currentModel;

        Shader* currentShader; // This would be the lighting pass shader
        Shader* outlineShader;
        Style currentStyle;

        // Passthrough shaders for final render
        Shader* passthroughShader;

        // Hard coded for now to prevent the function from being overcomplicated
        const unsigned int SCR_WIDTH = 1600;
        const unsigned int SCR_HEIGHT = 1200;

        unsigned int gBuffer;
        unsigned int gPosition, gNormal, gAlbedoSpec;
        unsigned int rboDepth;
        Shader* gBufferShader;

        unsigned int lightingFBO;
        unsigned int lightingColorTextures[2]; // FragColor and BrightColor

        // Bloom
        Shader* bloomShader;
        unsigned int bloomFBO[2];
        unsigned int bloomTexture[2];

        // HDR 
        Shader* hdrShader;
        unsigned int hdrFBO;
        unsigned int hdrTexture;

        // Final Pass
        unsigned int finalPassFBO;
        unsigned int finalPassTexture;

        void selectStyle(Style shaderStyle, ShaderParams& params, enginemath::Mat4 projection,
        enginemath::Mat4 view, enginemath::Vec3 cameraPos, enginemath::Mat4 modelMatrix) {
            switch (shaderStyle) {
                case (CARTOON): {
                    if (currentStyle != shaderStyle || currentShader == nullptr) {
                        delete currentShader;
                        currentShader = new Shader("shaders/Stylized Shaders/toon/toon.vert", "shaders/Stylized Shaders/toon/toon.frag");
                        currentStyle = shaderStyle;
                    }

                    break;
                }
                case (ANIME): {
                    if (currentStyle != shaderStyle || currentShader == nullptr) {
                        delete currentShader;
                        currentShader = new Shader("shaders/Stylized Shaders/anime/anime.vert", "shaders/Stylized Shaders/anime/anime.frag");
                        currentStyle = shaderStyle;
                    }

                    // Geometry pass
                    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    gBufferShader->use();
                    gBufferShader->setMat4("projection", projection);
                    gBufferShader->setMat4("view", view);
                    gBufferShader->setMat4("model", modelMatrix);
                    currentlyLoaded->Draw(*gBufferShader);
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    // Lighting Pass
                    glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO);
                    glClear(GL_COLOR_BUFFER_BIT);

                    // Bind textures from the geometry pass
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, gPosition);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, gNormal);
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

                    currentShader->use();
                    currentShader->setInt("geometryPass.gPosTexture", 0);
                    currentShader->setInt("geometryPass.gNormalTexture", 1);
                    currentShader->setInt("geometryPass.gAlbedoSpecTexture", 2);

                    currentShader->setVec3("uCameraPos", cameraPos);
                    currentShader->setVec3("uDirectionalLight", enginemath::Vec3(params.dirLight[0], params.dirLight[1], params.dirLight[2]));
                    currentShader->setVec4("uSpecular", enginemath::Vec4(params.specularColor[0], params.specularColor[1], params.specularColor[2], params.specularColor[3]));
                    currentShader->setFloat("uGlossiness", params.glossiness);
                    currentShader->setBool("bRimColor", params.bRimColor);
                    currentShader->setVec4("uRimColor", enginemath::Vec4(params.rimColor[0], params.rimColor[1], params.rimColor[2], params.rimColor[3]));
                    renderer->drawQuad();
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    // Copy lightingColorTextures[0] into finalPassTexture
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, lightingFBO);
                    glReadBuffer(GL_COLOR_ATTACHMENT0);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalPassFBO);
                    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
                    
                    // Bloom Pass
                    for (int i = 0; i < 2; i++) {
                        glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[i]);
                        glClear(GL_COLOR_BUFFER_BIT);
                    }
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    bool horizontal = false;
                    if (params.bBloom) {
                        bloomShader->use();
                        // Image is the "bright" lights from lighting pass
                        bloomShader->setInt("image", 0);
                        for (unsigned int i = 0; i < params.blurPasses; i++) {
                            glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[horizontal]);
                            bloomShader->setBool("isHorizontal", horizontal);
                            glActiveTexture(GL_TEXTURE0);
                            glBindTexture(GL_TEXTURE_2D, i == 0 ? lightingColorTextures[1] : bloomTexture[!horizontal]);
                            renderer->drawQuad();
                            horizontal = !horizontal;
                        }
                    }
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    // HDR Pass
                    if (params.bHDR) {
                        hdrShader->use();
                        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
                        glClear(GL_COLOR_BUFFER_BIT);

                        // Pass textures from Bloom
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, lightingColorTextures[0]);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, bloomTexture[!horizontal]);

                        hdrShader->setFloat("exposure", params.exposure);
                        hdrShader->setInt("hdrBuffer", 0);
                        hdrShader->setInt("bloomBlur", 1);
                        renderer->drawQuad();

                        glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrFBO);
                        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, finalPassFBO);
                        glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
                    }
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);


                    passthroughShader->use();
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, finalPassTexture); // from lighting pass, maybe move it to a general buffer
                    passthroughShader->setInt("finalTextureImage", 0);
                    renderer->drawQuad();

                    // Copy depth from gBuffer to default framebuffer
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


                    if (params.bOutline) {
                        if (outlineShader == nullptr) {
                            outlineShader = new Shader("shaders/Stylized Shaders/anime/outline.vert", "shaders/Stylized Shaders/anime/outline.frag");
                        }
                        outlineShader->use();
                        outlineShader->setMat4("projection", projection);
                        outlineShader->setMat4("view", view);
                        outlineShader->setMat4("model", modelMatrix);
                        outlineShader->setFloat("uOutlineWidth", params.outlineWidth);
                        glCullFace(GL_FRONT);
                        currentlyLoaded->Draw(*outlineShader);
                        glCullFace(GL_BACK);
                    }

                    break;
                }
            };
        }

        void shaderWithOutline(Shader& shader,  Model& model) {
            shader.use();
            glCullFace(GL_FRONT);
            model.Draw(shader);

            glCullFace(GL_BACK);
        }


};

#endif
