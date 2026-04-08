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
#include <random>

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
const float PARTICLE_RADIUS = 0.2f;

// ===== Structs =====
struct Particle{
    float radius;
    unsigned int segmants = 100;

    // Position
    float pos_x;
    float pos_y;

    // Velocity Data
    float velocity_x;
    float velocity_y;

    Particle(float x, float y, float r)
    {
        radius = r;
        pos_x = x;
        pos_y = y;

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dist(0.0f, 2.0f * PI);

        float angle = dist(gen);
        float dir_x = cos(angle);
        float dir_y = sin(angle);
        float speed = 0.5f;

        velocity_x = dir_x * speed;
        velocity_y = dir_y * speed;
    }
};

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

// Collision Functions
bool check_collision(Particle &p1, Particle &p2)
{
    float dx = p1.pos_x - p2.pos_x;
    float dy = p1.pos_y - p2.pos_y;

    float distance = sqrt((dx*dx) + (dy*dy));

    return distance <= (p1.radius + p2.radius);
}

void collision_response(Particle &p1, Particle &p2)
{
    // Get direction between centers
    float dx = p2.pos_x - p1.pos_x;
    float dy = p2.pos_y - p1.pos_y;

    // Get distance between centers
    float distance = sqrt((dx * dx) + (dy * dy));
    if(distance == 0)
    {
        return;
    }

    // Get the collision normal (the line along which the collision happens)
    float norm_x = dx / distance;
    float norm_y = dy / distance;

    // Overlap
    float overlap = (p1.radius + p2.radius) - distance;
    if(overlap > 0)
    {
        p1.pos_x = p1.pos_x - ((overlap / 2) * norm_x);
        p1.pos_y = p1.pos_y - ((overlap / 2) * norm_y);
        p2.pos_x = p2.pos_x + ((overlap / 2) * norm_x);
        p2.pos_y = p2.pos_y + ((overlap / 2) * norm_y);
    }

    // Relative velocity
    float rel_velocity_x = p2.velocity_x - p1.velocity_x;
    float rel_velocity_y = p2.velocity_y - p1.velocity_y;

    // Velocity along normal
    glm::vec2 rel_velocity = glm::vec2(rel_velocity_x, rel_velocity_y);
    glm::vec2 norm_position = glm::vec2(norm_x, norm_y);
    float velo_along_norm = glm::dot(rel_velocity, norm_position);

    // Check if moving toward or apart
    if (velo_along_norm > 0)
    {
        return;
    }

    // Correct velocity
    float velo_correct_x = -velo_along_norm * norm_x;
    float velo_correct_y = -velo_along_norm * norm_y;

    p1.velocity_x -= velo_correct_x;
    p1.velocity_y -= velo_correct_y;
    p2.velocity_x += velo_correct_x;
    p2.velocity_y += velo_correct_y;
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
    Particle p1(-0.5f, 0.0f, PARTICLE_RADIUS);
    Particle p2(0.5f, 0.0f, PARTICLE_RADIUS);

    // Aspect Ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    // Vertex Data
    vector<glm::vec2> particleMesh = create_circle(p1.radius, p1.segmants, aspect);

    // --- CONFIGURE CUBE VAO (AND VBO) ---
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, particleMesh.size() * sizeof(glm::vec2), particleMesh.data(), GL_STATIC_DRAW);

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
        glfwGetFramebufferSize(window, &width, &height);
        aspect = (float)width / (float)height;

        // --- MOVEMENT ---
        // Update circle position
        p1.pos_x = p1.pos_x + p1.velocity_x * delta_time;
        p1.pos_y = p1.pos_y + p1.velocity_y * delta_time;
        p2.pos_x = p2.pos_x + p2.velocity_x * delta_time;
        p2.pos_y = p2.pos_y + p2.velocity_y * delta_time;

        // p1 X boundaries
        if (p1.pos_x + PARTICLE_RADIUS >= aspect)
        {
            p1.pos_x = aspect - PARTICLE_RADIUS;
            p1.velocity_x *= -1.0f;
        }
        else if (p1.pos_x - PARTICLE_RADIUS <= -aspect)
        {
            p1.pos_x = -aspect + PARTICLE_RADIUS;
            p1.velocity_x *= -1.0f;
        }

        // p1 Y boundaries
        if (p1.pos_y + PARTICLE_RADIUS >= 1.0f)
        {
            p1.pos_y = 1.0f - PARTICLE_RADIUS;
            p1.velocity_y *= -1.0f;
        }
        else if (p1.pos_y - PARTICLE_RADIUS <= -1.0f)
        {
            p1.pos_y = -1.0f + PARTICLE_RADIUS;
            p1.velocity_y *= -1.0f;
        }

        // p2 X boundaries
        if (p2.pos_x + PARTICLE_RADIUS >= aspect)
        {
            p2.pos_x = aspect - PARTICLE_RADIUS;
            p2.velocity_x *= -1.0f;
        }
        else if (p2.pos_x - PARTICLE_RADIUS <= -aspect)
        {
            p2.pos_x = -aspect + PARTICLE_RADIUS;
            p2.velocity_x *= -1.0f;
        }

        // p1 Y boundaries
        if (p2.pos_y + PARTICLE_RADIUS >= 1.0f)
        {
            p2.pos_y = 1.0f - PARTICLE_RADIUS;
            p2.velocity_y *= -1.0f;
        }
        else if (p2.pos_y - PARTICLE_RADIUS <= -1.0f)
        {
            p2.pos_y = -1.0f + PARTICLE_RADIUS;
            p2.velocity_y *= -1.0f;
        }

        // Collisions
        if(check_collision(p1, p2) == true)
        {
            collision_response(p1, p2);
        }

        // --- ORTHOGRAPHIC PROJECT MATRIX ---
        glm::mat4 projection = glm::mat4(1.0f); // initialize matrix to identity matrix first
        projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

        mainShader.use();
        unsigned int projectionLoc = glGetUniformLocation(mainShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // --- FIRST PARTICLE ---
        glm::mat4 transform = glm::mat4(1.0f); // initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(p1.pos_x, p1.pos_y, 0.0f));

        unsigned int transformLoc = glGetUniformLocation(mainShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, particleMesh.size());

        // --- SECOND PARTICLE ---
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(p2.pos_x, p2.pos_y, 0.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, particleMesh.size());

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