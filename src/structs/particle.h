#pragma once

#include <glm/glm.hpp>

struct Particle
{
    // Vertex Data
    float radius;
    unsigned int segmants;

    // Position
    glm::vec2 position;

    // Velocity
    glm::vec2 velocity;

    // Acceleration
    glm::vec2 acceleration;

    // Mass
    float mass;

    // Restitution
    float e;

    Particle(float r, unsigned int segs, glm::vec2 pos, glm::vec2 velo, glm::vec2 accel, float p_mass, float p_e)
    {
        radius = r;
        segmants = segs;
        position = pos;
        velocity = velo;
        acceleration = accel;
        mass = p_mass;
        e = p_e;
    }
};