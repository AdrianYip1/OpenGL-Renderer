#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <shader/shader.hpp>
#include "vertex.h"
#include "texture.hpp"

class Mesh {
    public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(Shader &shader);
    void bind() const;
    bool checkElementArray() const;
    unsigned int getIndices() const;
    unsigned int getVertices() const;

    private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif
