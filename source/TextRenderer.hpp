#ifndef __TEXT_RENDERER_HPP__
#define __TEXT_RENDERER_HPP__

#include <map>
#include <vector>

#include "glew-1.9.0\glew.h"
#include "glm-0.9.7.5\glm\glm.hpp"
#include "glm-0.9.7.5\glm\gtc\matrix_transform.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Utility.hpp"
#include "Shader.hpp"

class Font
{
public:
    Font() = delete;
    Font(const Font&) = delete;
    Font(Font&&) = default;
    Font& operator=(const Font&) = delete;
    Font& operator=(Font&&) = default;

    struct Character
    {
        GLuint textureId;	// ID handle of the glyph texture
        glm::ivec2 size;	// Size of glyph
        glm::ivec2 bearing;	// Offset from baseline to left/top of glyph
        GLuint advance;		// Horizontal offset to advance to next glyph
    };

    std::uint32_t getFontSize() { return fontSize; }
    std::uint32_t getLineHeight() { return fontSize; }

    Font(FT_Library &ft, std::string &path, int fontSize) : fontSize(fontSize)
    {
        FT_Face face;
        if (FT_New_Face(ft, path.c_str(), 0, &face))
        {
            log("Failed to load font.");
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (GLubyte c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                log("Failed to load glyph.");
                continue;
            }
            // Generate texture
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
            // Set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character =
            {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<GLint>(face->glyph->advance.x)
            };
            characters.insert(std::pair<GLchar, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        // Destroy FreeType once we're finished
        FT_Done_Face(face);
    }

    std::map<GLchar, Character>& getCharacters() { return characters; }
private:
    std::uint32_t fontSize;
    std::uint32_t lineHeight;
    std::map<GLchar, Character> characters;
};

class TextRenderer
{
public:
    TextRenderer() = delete;
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = default;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer& operator=(TextRenderer&&) = default;

    TextRenderer(
        std::vector<std::pair<std::string, int>>&& fontsData,
        glm::mat4 &projection,
        std::shared_ptr<Shader> shader)
        : projection(projection),
        shader(shader)
    {
        initializeFreetype(std::move(fontsData));

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, 0, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void renderText(Font &font, std::string&& text, glm::vec2 position, glm::vec3 color)
    {
        _renderText(font, std::move(text), position, color);
    }

    void renderText(std::string&& text, glm::vec2 position, glm::vec3 color)
    {
        _renderText(fonts.at(0), std::move(text), position, color);
    }

    void renderText(std::string &text, glm::vec2 position, glm::vec3 color)
    {
        _renderText(fonts.at(0), text, position, color);
    }

    std::vector<Font>& getFonts() { return fonts; }

private:
    glm::mat4 projection;
    std::vector<Font> fonts;
    GLuint vao, vbo;

    std::shared_ptr<Shader> shader;

    void initializeFreetype(std::vector<std::pair<std::string, int>>&& fontsData)
    {
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            log("Could not initialize FreeType Library.");

        //for (std::pair<std::string&&, int> info : fontsData)
        //    fonts.push_back(Font(ft, info.first, info.second));

        for (int i = 0; i < fontsData.size(); i++)
            fonts.push_back(Font(ft, fontsData[i].first, fontsData[i].second));
        
        FT_Done_FreeType(ft); 
    }

    void _renderText(Font &font, std::string &text, glm::vec2 position, glm::vec3 color)
    {
        shader->use();
        shader->setUniform("textColor", color);
        shader->setUniform("projection", projection);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);

        glm::vec2 startingPosition = position;

        for (std::uint32_t i = 0; i < text.length(); i++)
        {
            if (text[i] == 10) // 10 is ASCII code for '\n'
            {
                position.y -= font.getLineHeight();
                position.x = startingPosition.x;
                continue;
            }

            Font::Character ch = font.getCharacters()[text[i]];

            GLfloat xpos = position.x + ch.bearing.x;
            GLfloat ypos = position.y - (ch.size.y - ch.bearing.y);

            GLfloat w = static_cast<GLfloat>(ch.size.x);
            GLfloat h = static_cast<GLfloat>(ch.size.y);

            // Update VBO for each character
            GLfloat vertices[6][4] =
            {
                { xpos, ypos + h, 0.0, 0.0 },
                { xpos, ypos, 0.0, 1.0 },
                { xpos + w, ypos, 1.0, 1.0 },

                { xpos, ypos + h, 0.0, 0.0 },
                { xpos + w, ypos, 1.0, 1.0 },
                { xpos + w, ypos + h, 1.0, 0.0 }
            };
            // Render glyph texture over quad
            glBindTexture(GL_TEXTURE_2D, ch.textureId);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            position.x += (ch.advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

#endif