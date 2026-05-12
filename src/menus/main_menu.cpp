#include "main_menu.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../core/shader.h"

#include "structs/button.h"

// Vertex Data
float iconVertices[] = {
    // x,  y
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f};

void setup_buffer_main_menu(unsigned int &mainMenuVAO, unsigned int &mainMenuVBO)
{
    glGenVertexArrays(1, &mainMenuVAO);
    glGenBuffers(1, &mainMenuVBO);

    glBindVertexArray(mainMenuVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mainMenuVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(iconVertices), iconVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void render_button(Button button, Shader &mainMenuShader, unsigned int mainMenuVAO)
{
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(button.position, 0.0f));
    model = glm::scale(model, glm::vec3(button.size, 1.0f));

    mainMenuShader.setMat4("model", model);
    mainMenuShader.setVec3("color", button.color);

    glBindVertexArray(mainMenuVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_main_menu(Shader &mainMenuShader, float aspect, unsigned int mainMenuVAO)
{
    // --- BUTTON DATA ---
    Button startButton;

    startButton.position = {300, 200};
    startButton.size = {200, 60};
    startButton.color = {0.2f, 0.7f, 0.3f};

    // --- ORTHOGRAPHIC PROJECT MATRIX ---
    glm::mat4 projection = glm::mat4(1.0f); // initialize matrix to identity matrix first
    projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    mainMenuShader.use();
    mainMenuShader.setMat4("projection", projection);

    render_button(startButton, mainMenuShader, mainMenuVAO);
}
