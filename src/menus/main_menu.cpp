#include "main_menu.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/shader.h"

#include "structs/button.h"

// --- VERTEX DATA ---
float iconVertices[] = {
    // x,  y
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,

    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f};

// --- BUTTONS ---
Button startButton;
Button quitButton;

// --- BUTTON SETTINGS ---
glm::vec2 buttonSize =  {200, 60};
glm::vec3 normalColor = {0.2f, 0.7f, 0.3f};
glm::vec3 hoverColor =  {0.3f, 0.9f, 0.4f};

// --- MAIN MENU FUNCTIONS ---
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
    startButton.position = {300, 270};
    startButton.size = buttonSize;
    startButton.color = normalColor;

    quitButton.position = {300, 200};
    quitButton.size = buttonSize;
    quitButton.color = normalColor;

    // --- ORTHOGRAPHIC PROJECT MATRIX ---
    glm::mat4 projection = glm::mat4(1.0f); // initialize matrix to identity matrix first
    projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

    mainMenuShader.use();
    mainMenuShader.setMat4("projection", projection);

    render_button(startButton, mainMenuShader, mainMenuVAO);
    render_button(quitButton, mainMenuShader, mainMenuVAO);
}

void update_main_menu(GLFWwindow *window, AppState &currentState)
{
    double mouseX, mouseY;

    // Get mouse position
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = WINDOW_HEIGHT - mouseY;

    // Check Start button
    if (startButton.is_hovered(mouseX, mouseY))
    {
        startButton.color = hoverColor;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            cout << "Starting simulation..." << endl;
            currentState = AppState::CollisionWithBalls;
        }
    }

    // Check Quit button
    if (quitButton.is_hovered(mouseX, mouseY))
    {
        quitButton.color = hoverColor;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            cout << "Closing..." << endl;
            glfwSetWindowShouldClose(window, true);
        }
    }
}