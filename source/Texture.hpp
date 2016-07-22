#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <string>
#include "glew-1.9.0\glew.h"
#include "soil-1.16\SOIL.h"

class Texture
{
public:
    
    // no default constructor
    // allow for moving but not for copying because of releasing memory in destructor

    Texture() = delete;
    Texture(const Texture&) = delete;
    Texture(Texture&&) = default;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = default;

    Texture(std::string&& filePath)
    {
        if (filePath.length() == 0)
            error("Filepath is empty.");

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int32 width, height;
        unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

        if (image == nullptr)
            error("Error loading image from file \"" + filePath + "\".");

        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type)
    {
        glGenTextures(1, &id);

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture()
    {
        if (id != -1)
            glDeleteTextures(1, &id);
    }

    GLuint getId() { return id; }

private:
    GLuint id = -1;
};

#endif