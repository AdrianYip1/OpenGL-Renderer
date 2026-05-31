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

class ModelSelect {
    public: 

        // Constructor that sets all the points to null
        ModelSelect() {
            currentlyLoaded = nullptr;
            currentModel = NONE_MODEL;

            currentShader = nullptr;
            currentStyle = NONE_STYLE;
        }

        void selectModel(ModelType model, Style shaderStyle, ShaderParams& params, enginemath::Mat4 projection,
        enginemath::Mat4 view) {
        
            switch (model) {
                case (GENSHIN): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/genshin/genshin.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 genshinModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f);
                    selectStyle(shaderStyle, params, projection, view, genshinModelMat);
                    break;
                }
                case (SAMUS): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/samus/samus.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 samusModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f) * enginemath::Mat4::scaleM(0.1f, 0.1f, 0.1f);
                    selectStyle(shaderStyle, params, projection, view, samusModelMat);
                    break;
                }
                case (TANGROWTH): {
                    if (currentModel != model || currentlyLoaded == nullptr) {
                        delete currentlyLoaded;
                        currentlyLoaded = new Model("models/tangrowth/tangrowth.fbx");
                        currentModel = model;
                    }
                    enginemath::Mat4 tanModelMat = enginemath::Mat4::translationM(0.0f, -1.0f, 0.0f) * enginemath::Mat4::scaleM(0.1f, 0.1f, 0.1f);
                    selectStyle(shaderStyle, params, projection, view, tanModelMat);
                    break;
                }

                default: break;
            };
        }


    private:
        Model* currentlyLoaded;
        ModelType currentModel;

        Shader* currentShader;
        Style currentStyle;

        void selectStyle(Style shaderStyle, ShaderParams& params, enginemath::Mat4 projection,
        enginemath::Mat4 view, enginemath::Mat4 modelMatrix) {
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