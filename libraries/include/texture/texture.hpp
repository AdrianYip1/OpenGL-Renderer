#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <cstring>
#include <ostream>
#include <iostream>



class Texture {
    public:
        Texture(const char* texturePath); // for 2d

        // the texture's uniform needs to be the same integer as the number in GL_TEXTURE(0-16)
        void bind(unsigned int unit);

    private:
        unsigned int texture;
        int width, height, nrChannels;
};

#endif