#pragma once

#include "structs/particle.h"
#include "core/shader.h"

// External Variables
extern const float PI;
extern const glm::vec2 g;

// Function Prototypes
void update_simulation(std::vector<Particle> &balls, float deltaTime, float aspect);
void render_simulation(Shader &shader, std::vector<Particle> &balls, int ballMeshSize, float aspect, unsigned int VAO);
std::vector<Particle> create_balls();