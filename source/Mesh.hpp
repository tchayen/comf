#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <string>
#include <vector>

#include "glew-1.9.0\glew.h"
#include "glm-0.9.7.5\glm\glm.hpp"
#include "tinyobjloader\TinyObjLoader.h"

class Mesh
{
public:
    Mesh() = delete;
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = default;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = default;

    Mesh(std::string&& file)
    {
        if (file.length() == 0)
            error("Filepath is empty.");

        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        tinyobj::LoadObj(shapes, materials, file.c_str());

        for (uint32 i = 0; i < shapes[0].mesh.positions.size(); i += 3)
            vertices.push_back(glm::vec3(shapes[0].mesh.positions[i], shapes[0].mesh.positions[i + 1], shapes[0].mesh.positions[i + 2]));

        for (uint32 i = 0; i < shapes[0].mesh.texcoords.size(); i += 2)
            uvs.push_back(glm::vec2(shapes[0].mesh.texcoords[i], shapes[0].mesh.texcoords[i + 1]));

        for (uint32 i = 0; i < shapes[0].mesh.normals.size(); i += 3)
            normals.push_back(glm::vec3(shapes[0].mesh.normals[i], shapes[0].mesh.normals[i + 1], shapes[0].mesh.normals[i + 2]));

        for (uint32 i = 0; i < shapes[0].mesh.indices.size(); i++)
            indices.push_back(shapes[0].mesh.indices[i]);

        // Fatal error - one of the vectors is empty and it will cause problems with OpenGL buffers. Program must be closed.
        if (vertices.size() == 0 || uvs.size() == 0 || normals.size() == 0)
        {
            std::string info = "";

            if (vertices.size() == 0)
            {
                info.append("vertices");
            }

            if (uvs.size() == 0)
            {
                if (info.length() != 0)
                    info.append(", ");

                info.append("uvs");
            }

            if (normals.size() == 0)
            {
                if (info.length() != 0)
                    info.append(", ");

                info.append("normals");
            }

            error("There are no " + info + " in mesh from file \"" + file + "\".");
        }

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vertexBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &uvBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferObject);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &normalBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &elementBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, elementBufferObject);
        glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned short int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvBufferObject);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

        glBindVertexArray(0);
    }

    ~Mesh()
    {
        glDeleteBuffers(1, &vertexBufferObject);
        glDeleteBuffers(1, &uvBufferObject);
        glDeleteBuffers(1, &normalBufferObject);
        glDeleteBuffers(1, &elementBufferObject);
        glDeleteVertexArrays(1, &vao);
    }

    void draw()
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<uint16> indices;

    GLuint vao;

    GLuint vertexBufferObject;
    GLuint uvBufferObject;
    GLuint normalBufferObject;
    GLuint elementBufferObject;
};
#endif