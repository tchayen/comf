#ifndef __GRAPHICS_HPP__
#define __GRAPHICS_HPP__

#include <stdexcept>

#include <glew-1.9.0\glew.h>

#include "glm-0.9.7.5\glm\glm.hpp"
#include "glm-0.9.7.5\glm\gtc\matrix_transform.hpp"

#include "Utility.hpp"

class Graphics
{
public:

    void initialize(
        GLFWwindow *&window,
        int32 width,
        int32 height,
        void (*scrollCallback)(GLFWwindow* window, double deltaX, double deltaY))
    {
        // GLFW
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window = glfwCreateWindow(width, height, "", nullptr, nullptr);

        if (window == nullptr)
        {
            error("Failed to create GLFW window.");
            glfwTerminate();
        }

        glfwMakeContextCurrent(window);

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetScrollCallback(window, (*scrollCallback));

        // GLEW
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            error("Failed to initialize GLEW.");
        }

        int32 w, h;
        glfwGetFramebufferSize(window, &w, &h);

        glViewport(0, 0, w, h);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    inline glm::mat4 getModelMatrix(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale)
    {
        glm::mat4 unit(1);
        return unit
            * glm::translate(unit, translate)
            * glm::rotate(unit, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f)) // x
            * glm::rotate(unit, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f)) // y
            * glm::rotate(unit, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f)) // z
            * glm::scale(unit, scale);
    }

    inline void setClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    inline void setClearColor(float a)
    {
        glClearColor(a, a, a, 1.0f);
    }

    inline void clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    inline void useTexture(int32 shaderId, int32 textureId, int32 offset, std::string &&uniformName)
    {
        glActiveTexture(GL_TEXTURE0 + offset);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glUniform1i(glGetUniformLocation(shaderId, uniformName.c_str()), 0);
    }
};

#endif