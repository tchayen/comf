#include <map>
#include <thread>
#include <string>

#include "glew-1.9.0\glew.h"
#include "glfw-3.2\glfw3.h"
#include "soil-1.16\SOIL.h"

#include "glm-0.9.7.5\glm\glm.hpp"
#include "glm-0.9.7.5\glm\gtc\matrix_transform.hpp"
#include "glm-0.9.7.5\glm\gtc\type_ptr.hpp"

#include "Utility.hpp"
#include "Graphics.hpp"
#include "TextRenderer.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"

// Variance shadow mapping is implemented but needs some improvements in order to fix "light bleeding".
// If you want to try it, uncomment #define below and in shaders/shadow_mapping.frag uncomment function and its call (and remove line below it).
//#define __VSM__

glm::mat4 projection;

int32 g_deltaY;
void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);

std::unique_ptr<TextRenderer> text;
std::unique_ptr<Graphics> g;

std::unique_ptr<Texture> shadowMapTexture, shadowMapDepthTexture, blurTexture;
std::unique_ptr<Framebuffer> shadowMapFbo, blurFbo;

std::shared_ptr<Shader> fontShader, simpleDepthShader, shader, blurShader, debugQuadShader;

std::unique_ptr<Mesh> m, plane;

std::unique_ptr<Texture> t1, t2;
std::unique_ptr<Camera> camera;

void render();
void update();

struct Window
{
    double mouseX, mouseY;
    int32 width = 1440, height = 810;
    GLFWwindow* glfwPointer = nullptr;
} window;

std::string gpuInfo;

const GLuint SHADOWMAP_SIZE = 1024;
GLuint quadVAO = 0, quadVBO = 0;

int32 main()
{
    srand(time(0));

    g->initialize(window.glfwPointer, 1440, 810, scrollCallback);
    projection = glm::ortho(0.0f, static_cast<GLfloat>(1440), 0.0f, static_cast<GLfloat>(810));

    gpuInfo = "GPU: " + std::string((char*)glGetString(GL_RENDERER)) + "\nDriver version: " + std::string((char*)glGetString(GL_VERSION));

    fontShader = std::make_unique<Shader>("", "shaders/font.frag");

    simpleDepthShader = std::make_unique<Shader>("shaders/shadow.vert", "shaders/shadow.frag");
    blurShader = std::make_unique<Shader>("shaders/blur.vert", "shaders/blur.frag");
    shader = std::make_unique<Shader>("shaders/shadow_mapping.vert", "shaders/shadow_mapping.frag");

    debugQuadShader = std::make_unique<Shader>("shaders/debug_quad.vert", "shaders/debug_quad.frag");

    text = std::make_unique<TextRenderer>(
        std::vector<std::pair<std::string, int32>>
        {
            { std::string("assets/roboto_light.ttf"), 22 },
            { std::string("assets/liberation_mono.ttf"), 16 }
        },
        projection,
        fontShader
    );

    m = std::make_unique<Mesh>("assets/scene.obj");
    plane = std::make_unique<Mesh>("assets/plane.obj");
    t1 = std::make_unique<Texture>("assets/textureA.png");
    t2 = std::make_unique<Texture>("assets/textureB.png");
    camera = std::make_unique<Camera>();

#ifdef __VSM__
    shadowMapTexture = std::make_unique<Texture>(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
    shadowMapFbo = std::make_unique<Framebuffer>(GL_COLOR_ATTACHMENT0, shadowMapTexture->getId());
    
    blurTexture = std::make_unique<Texture>(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
    blurFbo = std::make_unique<Framebuffer>(GL_COLOR_ATTACHMENT0, blurTexture->getId());
#endif

    while (!glfwWindowShouldClose(window.glfwPointer))
    {
        using clock = std::chrono::steady_clock;
        clock::time_point nextTimePoint = clock::now() + std::chrono::milliseconds(16);

        glfwPollEvents();

        render();
        update();

        std::this_thread::sleep_until(nextTimePoint);
    }

    glfwTerminate();
    return 0;
}

void renderScene(std::shared_ptr<Shader> s)
{
    
    s->setUniform(
        "model",
        g->getModelMatrix(glm::vec3(-5.0f, 0.0f, -5.0f), glm::vec3(0.0f), glm::vec3(3.0f)));

    plane->draw();
    
    /*
    for (int32 i = 0; i < positions.size(); i++)
    {
        s->setUniform(
            "model",
            g->getModelMatrix(positions[i], glm::vec3(0.0f, glm::radians(250.0f), 0.0f), glm::vec3(1.0f)));

        m->draw();
    }
    */

    s->setUniform(
        "model",
        g->getModelMatrix(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 180.0f, 0.0f), glm::vec3(1.0f)));

    m->draw();
}

void renderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // positions        // texture coordinates
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        // Setup plane VAO.
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void render()
{
    g->setClearColor(0);
    g->clear();

    glm::vec3 lightPosition(glm::vec3(-3.0f, 6.0f, -3.0f));
    
    // Light calculations.
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    GLfloat near_plane = 1.0f, far_plane = 30.0f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(1.0));
    lightSpaceMatrix = lightProjection * lightView;
    
#ifdef __VSM__

    // Render scene from light's point of view.
    shadowMapFbo->bind();
    glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    simpleDepthShader->use();
    simpleDepthShader->setUniform("lightSpaceMatrix", lightSpaceMatrix);

    glCullFace(GL_FRONT);
    renderScene(simpleDepthShader);
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    blurShader->use();
    glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
    
    // Blur shadowMapTexture.
    glDisable(GL_DEPTH_TEST);
    blurShader->use();
    
    for (int32 i = 0; i < 1; i++)
    {
        // Blur shadowMapTexture (horizontally) to blurTexture.
        blurFbo->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowMapTexture->getId());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blurShader->setUniform("horizontal", true);
        renderQuad();

        // Blur blurTexture vertically and write to shadowMapTexture.
        shadowMapFbo->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blurTexture->getId());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        blurShader->setUniform("horizontal", false);
        renderQuad();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

#endif

    glViewport(0, 0, window.width, window.height);
    glClearColor(0.412f, 0.733f, 0.929f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->use();

    shader->setUniform("projection", camera->getProjection());
    shader->setUniform("view", camera->getView());
    
    shader->setUniform("viewPosition", camera->position);
    shader->setUniform("lightSpaceMatrix", lightSpaceMatrix);

    shader->setUniform("light.direction", lightPosition);
    shader->setUniform("light.ambient", glm::vec3(0.2f, 0.23f, 0.25f));
    shader->setUniform("light.diffuse", glm::vec3(0.6f, 0.63f, 0.65f));
    shader->setUniform("light.specular", glm::vec3(0.0f));

    // TODO: switch to g->useTexture(...)
    shader->setUniform("diffuseTexture", 0);
    
#ifdef __VSM__
    shader->setUniform("shadowMap", 1);
#endif

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t1->getId());
    glActiveTexture(GL_TEXTURE1);

#ifdef __VSM__
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture->getId());
#endif

    renderScene(shader);

    // Comment beginning and end of the comment block (-> ///* ... //*/) and switch back and forward as you need.
    // You can customize this debug quad by choosing texture and altering its shader.
    /*
    debugQuadShader->use();
    glUniform1f(glGetUniformLocation(debugQuadShader->getId(), "near_plane"), near_plane);
    glUniform1f(glGetUniformLocation(debugQuadShader->getId(), "far_plane"), far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowMapTexture->getId());
    renderQuad();
    */

    text->renderText("* Test of .ttf font rendering *", glm::vec2(10, 810 - 25), glm::vec3(1));
    text->renderText(text->getFonts()[1], "can load several fonts", glm::vec2(10, 810 - 45), glm::vec3(1));

    glfwSwapBuffers(window.glfwPointer);
}

void update()
{
    if (glfwGetKey(window.glfwPointer, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window.glfwPointer, GL_TRUE);

    glm::vec3 movement = glm::vec3(0.0f, 0.0f, 0.0f);

    if (glfwGetKey(window.glfwPointer, GLFW_KEY_W) == GLFW_PRESS)
        movement += camera->getForwardVector();

    if (glfwGetKey(window.glfwPointer, GLFW_KEY_S) == GLFW_PRESS)
        movement -= camera->getForwardVector();

    if (glfwGetKey(window.glfwPointer, GLFW_KEY_A) == GLFW_PRESS)
        movement -= camera->getRightVector();

    if (glfwGetKey(window.glfwPointer, GLFW_KEY_D) == GLFW_PRESS)
        movement += camera->getRightVector();

    if (movement.x != 0 || movement.y != 0 || movement.z != 0)
        movement = glm::normalize(movement);

    movement = camera->moveSpeed * movement;
    camera->position += movement;

    double mouseX, mouseY;
    glfwGetCursorPos(window.glfwPointer, &mouseX, &mouseY);

    if (glfwGetMouseButton(window.glfwPointer, 1) == GLFW_MOUSE_BUTTON_RIGHT)
    {
        camera->offsetOrientation(camera->mouseSensitivity * (mouseY - window.mouseY), camera->mouseSensitivity * (mouseX - window.mouseX));
        glfwSetCursorPos(window.glfwPointer, window.mouseX, window.mouseY); //reset the mouse, so it doesn't go out of the window
    }
    else
    {
        window.mouseX = (float)mouseX;
        window.mouseY = (float)mouseY;
    }

    camera->changeDistance(g_deltaY * camera->distanceSpeed);
    g_deltaY = 0;
}

void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
{
    g_deltaY = deltaY;
}