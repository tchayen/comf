#ifndef __FRAMEBUFFER_HPP__
#define __FRAMEBUFFER_HPP__

#include "glew-1.9.0\glew.h"

#include "Utility.hpp"

class Framebuffer
{
public:

    // no default constructor
    // allow for moving but not for copying because of releasing memory in destructor

    Framebuffer() = delete;
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer(Framebuffer&&) = default;
    Framebuffer& operator=(const Framebuffer&) = delete;
    Framebuffer& operator=(Framebuffer&&) = default;

    Framebuffer(GLuint attachment, GLuint texture)
    {
        glGenFramebuffers(1, &id);
        glBindFramebuffer(GL_FRAMEBUFFER, id);

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);

        GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (GL_FRAMEBUFFER_COMPLETE != result)
            log("Framebuffer is not complete.");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~Framebuffer()
    {
        if (id != -1)
            glDeleteFramebuffers(1, &id);
    }

    inline void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    GLuint getId() { return id; }

private:
    GLuint id = -1;
};

#endif