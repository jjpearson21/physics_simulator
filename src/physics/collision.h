#pragma once

#include "../structs/particle.h"

// Function Prototypes
bool check_collision(Particle &p1, Particle &p2);
void collision_response(Particle &p1, Particle &p2);