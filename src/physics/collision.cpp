#include "collision.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <cmath>
#include <random>

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
    if (distance == 0)
        return;

    // Get the collision normal (the line along which the collision happens)
    glm::vec2 norm = dPos / distance;

    // Overlap
    float overlap = (p1.radius + p2.radius) - distance;
    float invMass1 = 1.0f / p1.mass;
    float invMass2 = 1.0f / p2.mass;
    float invMassSum = invMass1 + invMass2;

    if (overlap > 0)
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
        return;

    // Restitution constant from each particle in the collision
    float e = std::min(p1.e, p2.e);

    // Calculate the impulse magnitude and turn it into a vector
    float j = (-(1.0f + e) * velo_along_norm) / ((1.0f / p1.mass) + (1.0f / p2.mass));
    glm::vec2 impulse = j * norm;

    p1.velocity -= impulse / p1.mass;
    p2.velocity += impulse / p2.mass;
}