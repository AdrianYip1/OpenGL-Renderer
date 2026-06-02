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
#include <stylesAndModels.hpp>



// modelSelect will contain methods for rendering specific models:
// Setting their shaders, textures, positions, and scaling

//ShaderParams will consist of the uniform information needed and boolian values for
// different effects such as outlining, bloom, hdr,...

class ModelSelect {
    public: 

        // Constructor that sets all the points to null
        ModelSelect() {
            currentlyLoaded = nullptr;
            currentModel = NONE_MODEL;

            currentShader = nullptr;
            outlineShader = nullptr;
            currentStyle = NONE_STYLE;
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
        Model* currentlyLoaded;
        ModelType currentModel;

        Shader* currentShader;
        Shader* outlineShader;
        Style currentStyle;

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

                    // Set up uniforms
                    currentShader->setMat4("projection", projection);
                    currentShader->setMat4("view", view);
                    currentShader->setMat4("model", modelMatrix);
                    currentShader->setVec3("uCameraPos", cameraPos);

                    currentShader->setVec3("uDirectionalLight", params.dirLight);

                    currentShader->setVec4("uSpecular", params.specular);
                    currentShader->setVec4("uRimColor", params.rimColor);
                    currentShader->setFloat("uGlossiness", params.glossiness);

                    // Render the model (1st pass)
                    shaderWithoutOutline(*currentShader, *currentlyLoaded);

                    if (params.outline) {
                        if (outlineShader == nullptr) {
                            outlineShader = new Shader("shaders/Stylized Shaders/anime/outline.vert", "shaders/Stylized Shaders/anime/outline.frag");
                        }
                        outlineShader->setMat4("projection", projection);
                        outlineShader->setMat4("view", view);
                        outlineShader->setMat4("model", modelMatrix);
                        outlineShader->setFloat("uOutlineWidth", params.outlineWidth);
                        shaderWithOutline(*outlineShader, *currentlyLoaded);
                    }

                    break;
                }
            };
        }

        void shaderWithoutOutline(Shader& mainShader,  Model& model) {
            mainShader.use();
            glCullFace(GL_BACK);
            model.Draw(mainShader);
        }

        void shaderWithOutline(Shader& shader,  Model& model) {
            shader.use();
            glCullFace(GL_FRONT);
            model.Draw(shader);

            glCullFace(GL_BACK);
        }

       
};

#endif