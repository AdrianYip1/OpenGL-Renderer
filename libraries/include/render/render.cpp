#include "render.hpp"

void Render::draw(const Mesh& mesh) {
    mesh.bind();

    if (mesh.checkElementArray()) {
        glDrawElements(GL_TRIANGLES, mesh.getIndices(), GL_UNSIGNED_INT, 0);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.getVertices());
    }
}

// Sets up the vertex coordinates (Mproj * Mview * Mmodel * Vlocal)
void Render::setPosition(const Shader& shader, const enginemath::Mat4& projectionM, 
                        const enginemath::Mat4& viewM, const enginemath::Mat4& modelM) {

    shader.setMat4("modelM", modelM);
    shader.setMat4("viewM", viewM);
    shader.setMat4("projectionM", projectionM);
}

void Render::endFrame(GLFWwindow* window) {

    glfwSwapBuffers(window);
    glfwPollEvents();
}