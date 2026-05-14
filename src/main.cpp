#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "main.h"

#include "core/shader.h"
#include "core/camera.h"

#include "menus/main_menu.h"

#include "physics/collision.h"

#include "settings/particles.h"

#include "simulations/collisions_with_balls.h"

#include "structs/particle.h"

using namespace std;

// Timing
float deltaTime = 0.0f; // time between current and last frame
float lastFrame = 0.0f;

// ===== Functions For Running Program =====

// Frame Functions
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, AppState currentState)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && currentState == AppState::MainMenu)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && currentState == AppState::CollisionWithBalls)
        currentState = AppState::Paused;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && currentState == AppState::Paused)
        currentState = AppState::MainMenu;
}

// Init Functions
void init_glfw()
{
    // Initialize and configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    cout << "GLFW Initialized..." << endl;
}

bool load_glad()
{
    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to load GLAD" << endl;
        return false;
    }

    cout << "Succesfully loaded GLAD..." << endl;
    return true;
}

// Creation Functions
GLFWwindow *create_window()
{
    // Create window with GLFW
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
        cout << "Failed to create GLFW window" << endl;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

vector<glm::vec2> create_circle(float radius, int segments, float aspect)
{
    vector<glm::vec2> vertices;

    // Center at origin
    vertices.push_back(glm::vec2(0.0f, 0.0f));

    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * PI * i / segments;

        float x = radius * cos(angle);
        float y = radius * sin(angle);

        vertices.push_back(glm::vec2(x, y));
    }

    return vertices;
}

// ===== MAIN =====
int main(void)
{
    // Initialize glfw
    init_glfw();

    // Create window
    GLFWwindow *window = create_window();

    // Set mouse mode
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // Load GLAD
    if (!load_glad())
        return 1;

    // Configure global openGL state
    glEnable(GL_DEPTH_TEST);

    // Set app state
    AppState currentState = AppState::MainMenu;

    // Build and Compile Main Shader
    Shader mainShader("shaders/vertex_shaders/v_shader.txt", "shaders/fragment_shaders/f_shader.txt");
    Shader mainMenuShader("shaders/vertex_shaders/v_shader_menu.txt", "shaders/fragment_shaders/f_shader_menu.txt");

    // --- DATA ---
    vector<Particle> balls = create_balls();

    // Aspect Ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    // Vertex Data
    vector<glm::vec2> ballMesh = create_circle(ballRadius, ballSegmants, aspect);

    // --- CONFIGURE CUBE VAO (AND VBO) ---
    unsigned int mainMenuVAO;
    unsigned int mainMenuVBO;
    setup_buffer_main_menu(mainMenuVAO, mainMenuVBO);

    unsigned int ballVAO;
    unsigned int ballVBO;
    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);

    // Particle 1
    glBindVertexArray(ballVAO);

    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, ballMesh.size() * sizeof(glm::vec2), ballMesh.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
    glEnableVertexAttribArray(0);

    // ===== Render Loop =====
    while (!glfwWindowShouldClose(window))
    {
        // --- Pre-Frame timem logic ---
        float current_frame = static_cast<float>(glfwGetTime());
        deltaTime = current_frame - lastFrame;
        lastFrame = current_frame;

        // --- INPUT ---
        processInput(window, currentState);

        // --- RENDER SCREEN ---
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- ASPECT RATIO ---
        glfwGetFramebufferSize(window, &width, &height);
        aspect = (float)width / (float)height;

        switch (currentState)
        {
        case AppState::MainMenu:
            update_main_menu(window, currentState);
            render_main_menu(mainMenuShader, aspect, mainMenuVAO);
            break;

        case AppState::CollisionWithBalls:
            update_simulation(balls, deltaTime, aspect);
            render_simulation(mainShader, balls, ballMesh.size(), aspect, ballVAO);
            break;

        case AppState::Paused:
            break;
        }

        // --- SWAP BUFFERS AND POLL IO EVENTS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- DE-ALLOCATE ALL RESOURCES ---
    glDeleteVertexArrays(1, &mainMenuVAO);
    glDeleteBuffers(1, &mainMenuVBO);
    glDeleteVertexArrays(1, &ballVAO);
    glDeleteBuffers(1, &ballVBO);

    glfwTerminate();

    return 0;
}