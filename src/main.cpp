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
// mathematical constants
const float PI = 3.14159265359f;

// window
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// timing
float delta_time = 0.0f; // time between current and last frame
float last_frame = 0.0f;

// Random Velocity Direction Generation
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> dist(0.0f, 2.0f * PI);
float angle1 = dist(gen);
float angle2 = dist(gen);

// particles
float p1_radius = 0.2f;
glm::vec2 p1_pos = glm::vec2(0.5f, 0.0f);
float p1_speed = 2.0f;
glm::vec2 p1_velo = glm::vec2(cos(angle1), sin(angle1)) * p1_speed;
float p1_mass = 1.0f;
float p1_e = 0.9f;

float p2_radius = 0.3f;
glm::vec2 p2_pos = glm::vec2(-0.5f, 0.0f);
float p2_speed = 2.0f;
glm::vec2 p2_velo = glm::vec2(cos(angle2), sin(angle2)) * p2_speed;
float p2_mass = 20.0f;
float p2_e = 1.0f;

// ===== Structs =====
struct Particle{
    // Vertex Data
    float radius;
    unsigned int segmants = 100;

    // Position
    glm::vec2 position;

    // Velocity
    glm::vec2 velocity;

    // Mass
    float mass;

    // Restitution
    float e;

    Particle(float r, glm::vec2 pos, glm::vec2 velo, float p_mass, float p_e)
    {
        radius = r;
        position = pos;
        velocity = velo;
        mass = p_mass;
        e = p_e;
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
    glm::vec2 dPos = p1.position - p2.position;

    return glm::dot(dPos, dPos) <= (p1.radius + p2.radius) * (p1.radius + p2.radius);
}

void collision_response(Particle &p1, Particle &p2)
{
    // Get direction between centers
    glm::vec2 dPos = p2.position - p1.position;

    // Get distance between centers
    float distance = length(dPos);
    if(distance == 0)
    {
        return;
    }

    // Get the collision normal (the line along which the collision happens)
    glm::vec2 norm = dPos / distance;

    // Overlap
    float overlap = (p1.radius + p2.radius) - distance;
    float invMass1 = 1.0f / p1.mass;
    float invMass2 = 1.0f / p2.mass;
    float invMassSum = invMass1 + invMass2;

    if(overlap > 0)
    {
        p1.position -= (overlap * (invMass1 / invMassSum)) * norm;
        p2.position += (overlap * (invMass2 / invMassSum)) * norm;
    }

    // Relative velocity
    glm::vec2 rel_velocity = p2.velocity - p1.velocity;

    // Velocity along normal
    float velo_along_norm = glm::dot(rel_velocity, norm);

    // Check if moving toward or apart
    if (velo_along_norm > 0)
    {
        return;
    }

    // Restitution constant from each particle in the collision
    float e = min(p1.e, p2.e);

    // Calculate the impulse magnitude and turn it into a vector
    float j = (-(1.0f + e) * velo_along_norm) / ((1.0f / p1.mass) + (1.0f / p2.mass));
    glm::vec2 impulse = j * norm;
    

    p1.velocity -= impulse / p1.mass;
    p2.velocity += impulse / p2.mass;
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
    Particle p1(p1_radius, p1_pos, p1_velo, p1_mass, p1_e);
    Particle p2(p2_radius, p2_pos, p2_velo, p2_mass, p2_e);

    // Aspect Ratio
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    // Vertex Data
    vector<glm::vec2> particle1 = create_circle(p1.radius, p1.segmants, aspect);
    vector<glm::vec2> particle2 = create_circle(p2.radius, p2.segmants, aspect);

    // --- CONFIGURE CUBE VAO (AND VBO) ---
    unsigned int VAO1, VAO2, VBO1, VBO2;
    glGenVertexArrays(1, &VAO1);
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &VBO2);

    // Particle 1
    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, particle1.size() * sizeof(glm::vec2), particle1.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
    glEnableVertexAttribArray(0);

    // Particle 2
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, particle2.size() * sizeof(glm::vec2), particle2.data(), GL_STATIC_DRAW);

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
        p1.position = p1.position + p1.velocity * delta_time;
        p2.position = p2.position + p2.velocity * delta_time;

        // p1 X boundaries
        if (p1.position.x + p1.radius >= aspect)
        {
            p1.position.x = aspect - p1.radius;
            p1.velocity.x *= -1.0f;
        }
        else if (p1.position.x - p1.radius <= -aspect)
        {
            p1.position.x = -aspect + p1.radius;
            p1.velocity.x *= -1.0f;
        }

        // p1 Y boundaries
        if (p1.position.y + p1.radius >= 1.0f)
        {
            p1.position.y = 1.0f - p1.radius;
            p1.velocity.y *= -1.0f;
        }
        else if (p1.position.y - p1.radius <= -1.0f)
        {
            p1.position.y = -1.0f + p1.radius;
            p1.velocity.y *= -1.0f;
        }

        // p2 X boundaries
        if (p2.position.x + p2.radius >= aspect)
        {
            p2.position.x = aspect - p2.radius;
            p2.velocity.x *= -1.0f;
        }
        else if (p2.position.x - p2.radius <= -aspect)
        {
            p2.position.x = -aspect + p2.radius;
            p2.velocity.x *= -1.0f;
        }

        // p1 Y boundaries
        if (p2.position.y + p2.radius >= 1.0f)
        {
            p2.position.y = 1.0f - p2.radius;
            p2.velocity.y *= -1.0f;
        }
        else if (p2.position.y - p2.radius <= -1.0f)
        {
            p2.position.y = -1.0f + p2.radius;
            p2.velocity.y *= -1.0f;
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
        transform = glm::translate(transform, glm::vec3(p1.position, 0.0f));

        unsigned int transformLoc = glGetUniformLocation(mainShader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, particle1.size());

        // --- SECOND PARTICLE ---
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(p2.position, 0.0f));

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, particle2.size());

        // --- SWAP BUFFERS AND POLL IO EVENTS ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- DE-ALLOCATE ALL RESOURCES ---
    glDeleteVertexArrays(1, &VAO1);
    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &VBO2);

    glfwTerminate();

    return 0;
}