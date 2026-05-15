#include "texture.hpp"

Texture::Texture(const char* texturePath) { //2d

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // S and T coords
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    const char* dot = strrchr(texturePath, '.');
    const char* fileType = (dot) ? dot + 1 : "";

    if (data) {
        if (strcmp(fileType, "jpg") == 0 || strcmp(fileType, "jpeg") == 0) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (strcmp(fileType, "png") == 0) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else {
            std::cout << "TEXTURE NEEDS TO BE .png OR .jpg" << std::endl;
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
		std::cout << "ERROR GENERATING TEXTURE" << std::endl;
	}

    stbi_image_free(data);
}

void Texture::bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
}