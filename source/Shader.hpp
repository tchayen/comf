#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include <string>

#include <glew-1.9.0\glew.h>
#include <glm-0.9.7.5\glm\glm.hpp>
#include "glm-0.9.7.5\glm\gtc\type_ptr.hpp"

#include "Utility.hpp"
#include "Texture.hpp"

class Shader
{
public:
    Shader() = delete;
    Shader(const Shader&) = default;
    Shader(Shader&&) = default;
    Shader& operator=(const Shader&) = default;
    Shader& operator=(Shader&&) = default;

    ~Shader()
    {
        if (id != -1)
            glDeleteProgram(id);
    }

    Shader(
        std::string&& vertexShaderFilePath, 
        std::string&& fragmentShaderFilePath)
    {
        GLuint vertexHandler, fragmentHandler;
        std::string vertexShaderFile = loadFile(std::move(vertexShaderFilePath));
        std::string fragmentShaderFile = loadFile(std::move(fragmentShaderFilePath));
        const GLchar *vertexShader = vertexShaderFile.c_str();
        const GLchar *fragmentShader = fragmentShaderFile.c_str();

        vertexHandler = compileShader(vertexShader, GL_VERTEX_SHADER);
        fragmentHandler = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

        id = glCreateProgram();
        glAttachShader(id, vertexHandler);
        glAttachShader(id, fragmentHandler);
        glLinkProgram(id);

        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(id, 512, 0, infoLog);
            log("Error in linking of shaders:\n" + std::string(infoLog));
        }

        glDeleteShader(vertexHandler);
        glDeleteShader(fragmentHandler);
    }

    inline void use() const
    {
        if (id == -1)
            log("Tried to use empty shader.");

        glUseProgram(id);
    }

    inline GLuint getId() const { return id; }

    inline GLint getUniformLocation(const std::string& name) const
    {
        return glGetUniformLocation(id, name.c_str());
    }

    // for l-value reference
    // using references to avoid copying strings
    inline void setUniform(const std::string &name, bool value) const
    {
        glUniform1i(getUniformLocation(std::move(name)), value);
    }

    inline void setUniform(const std::string &name, int value) const
    {
        glUniform1i(getUniformLocation(std::move(name)), value);
    }

    inline void setUniform(const std::string &name, float value) const
    {
        glUniform1f(getUniformLocation(name), value);
    }

    inline void setUniform(const std::string &name, glm::vec2 value) const
    {
        glUniform2f(getUniformLocation(name), value.x, value.y);
    }

    inline void setUniform(const std::string &name, glm::vec3 value) const
    {
        glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
    }

    inline void setUniform(const std::string &name, glm::mat4 value) const
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
    }

    // for r-value references

    inline GLint getUniformLocation(std::string&& name) const
    {
        return glGetUniformLocation(id, name.c_str());
    }

    inline void setUniform(std::string&& name, bool value) const
    {
        glUniform1i(getUniformLocation(std::move(name)), value);
    }

    inline void setUniform(std::string&& name, int value) const
    {
        glUniform1i(getUniformLocation(std::move(name)), value);
    }

    inline void setUniform(std::string&& name, float value) const
    {
        glUniform1f(getUniformLocation(std::move(name)), value);
    }

    inline void setUniform(std::string&& name, glm::vec2 value) const
    {
        glUniform2f(getUniformLocation(std::move(name)), value.x, value.y);
    }

    inline void setUniform(std::string&& name, glm::vec3 value) const
    {
        glUniform3f(getUniformLocation(std::move(name)), value.x, value.y, value.z);
    }

    inline void setUniform(std::string&& name, glm::mat4 value) const
    {
        glUniformMatrix4fv(getUniformLocation(std::move(name)), 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    GLuint id = -1;
    GLint success;
    GLchar infoLog[512];

    GLuint compileShader(const GLchar *source, GLuint shaderType)
    {
        GLuint shaderHandler;

        shaderHandler = glCreateShader(shaderType);
        glShaderSource(shaderHandler, 1, &source, 0);
        glCompileShader(shaderHandler);
        
        // Print compile errors if there are any
        glGetShaderiv(shaderHandler, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderHandler, 512, 0, infoLog);
            log("Error in compilation of shader:\n" + std::string(infoLog));
        };

        return shaderHandler;
    }
};

#endif