#include "collisions_with_balls.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../core/shader.h"

#include "../structs/particle.h"

#include "../physics/collision.h"

#include <random>

void update_simulation(std::vector<Particle> &balls, float deltaTime, float aspect)
{
    // --- MOVEMENT ---
    for (Particle &ball : balls)
    {
        // Update circle position and velocity
        ball.velocity += ball.acceleration * deltaTime;
        ball.position += ball.velocity * deltaTime;

        // X boundaries
        if (ball.position.x + ball.radius >= aspect)
        {
            ball.position.x = aspect - ball.radius;
            ball.velocity.x *= -1.0f;
        }
        else if (ball.position.x - ball.radius <= -aspect)
        {
            ball.position.x = -aspect + ball.radius;
            ball.velocity.x *= -1.0f;
        }

        // Y boundaries
        if (ball.position.y + ball.radius >= 1.0f)
        {
            ball.position.y = 1.0f - ball.radius;
            ball.velocity.y *= -1.0f;
        }
        else if (ball.position.y - ball.radius <= -1.0f)
        {
            ball.position.y = -1.0f + ball.radius;
            ball.velocity.y *= -1.0f;
        }
    }

    // -- COLLISIONS --
    for (int i = 0; i < balls.size(); i++)
    {
        for (int j = i + 1; j < balls.size(); j++)
        {
            if (check_collision(balls[i], balls[j]))
            {
                collision_response(balls[i], balls[j]);
            }
        }
    }
}

void render_simulation(Shader &shader, std::vector<Particle> &balls, int ballMeshSize, float aspect, unsigned int VAO)
{
    // --- ORTHOGRAPHIC PROJECT MATRIX ---
    glm::mat4 projection = glm::mat4(1.0f); // initialize matrix to identity matrix first
    projection = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);

    shader.use();
    unsigned int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // --- RENDER BALLS ---
    for (Particle &ball : balls)
    {
        glm::mat4 transform = glm::mat4(1.0f); // initialize matrix to identity matrix first
        transform = glm::translate(transform, glm::vec3(ball.position, 0.0f));

        unsigned int transformLoc = glGetUniformLocation(shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, ballMeshSize);
    }
}

std::vector<Particle> create_balls()
{
    std::vector<Particle> balls;

    // Ball Properties
    float ballRadius = 0.2f;
    unsigned int ballSegmants = 100;
    float ballSpeed = 2.0f;
    float ballMass = 1.0f;
    float ballRestitution = 1.0f;
    glm::vec2 ballAccel = g;

    for (int i = 0; i < 10; i++)
    {
        // Random Velocity Direction Generation
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<float> dist(0.0f, 2.0f * PI);
        float angle = dist(gen);

        glm::vec2 ballPos = glm::vec2(cos(angle), sin(angle)) * 2.0f;
        glm::vec2 ballVelo = glm::vec2(cos(angle), sin(angle)) * ballSpeed;

        balls.push_back(Particle(ballRadius, ballSegmants, ballPos, ballVelo, ballAccel, ballMass, ballRestitution));
    }

    return balls;
}