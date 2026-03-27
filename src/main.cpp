#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

#include <iostream>
#include <cmath>

using namespace std;

// ===== Settings =====

// window
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// timing
float delta_time = 0.0f; // time between current and last frame
float last_frame = 0.0f;

// constants
const float PI = 3.14159265359f;

// ===== Functions For Running Program =====

// Frame Functions
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
    {
        cout << "Failed to create GLFW window" << endl;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

unsigned int load_texture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

vector<glm::vec2> create_circle(float centerX, float centerY, float radius, int segments)
{
    vector<glm::vec2> vertices;

    // Center
    vertices.push_back(glm::vec2(centerX, centerY));

    for (int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * PI * i / segments;

        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);

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

    // Load GLAD
    if (!load_glad())
        return 1;

    // Configure global openGL state
    glEnable(GL_DEPTH_TEST);

    // Build and Compile Main Shader
    Shader mainShader("shaders/v_shader.txt", "shaders/f_shader.txt");

    // --- DATA ---
    // Circle Data
    float radius = 0.2f;
    unsigned int segmants = 100;

    // Position Data
    float circle_pos_x = 0.0f;
    float circle_pos_y = 0.0f;
    float velocity_x = 0.5f;
    float velocity_y = 0.5f;

    // Vertex Data
    vector<glm::vec2> circle = create_circle(circle_pos_x, circle_pos_y, radius, segmants);

    // --- CONFIGURE CUBE VAO (AND VBO) ---
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, circle.size() * sizeof(glm::vec2), circle.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
    glEnableVertexAttribArray(0);

    // ===== Render Loop =====
    while (!glfwWindowShouldClose(window))
    {
        // --- Pre-Frame timem logic ---
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        // --- INPUT ---
        processInput(window);

        // --- RENDER SCREEN ---
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- ASPECT RATIO ---
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (float)width / (float)height;

        // --- MOVEMENT ---
        // Update circle position
        circle_pos_x = circle_pos_x + velocity_x * delta_time;

        // Boundary Checks
        if (circle_pos_x + radius >= 1.0f)
        {
            circle_pos_x = 1.0f - radius;
            velocity_x *= -1.0f;
        }
        else if (circle_pos_x - radius <= -1.0f)
        {
            circle_pos_x = -1.0f + radius;
            velocity_x *= -1.0f;
        }

        // --- TRANSFORMATIONS ---
        glm::mat4 transform = glm::mat4(1.0f); // initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(circle_pos_x, circle_pos_y, 0.0f));

        // --- SHADER STUFF ---
        mainShader.use();
        unsigned int aspectLoc = glGetUniformLocation(mainShader.ID, "aspect");
        unsigned int transformLoc = glGetUniformLocation(mainShader.ID, "transform");
        glUniform1f(aspectLoc, aspect);
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        // --- DRAW CIRCLE ---
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, circle.size());

        // --- SWAP BUFFERS AND POLL IO EVENTS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- DE-ALLOCATE ALL RESOURCES ---
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}