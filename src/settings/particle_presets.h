#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "../settings/constants.h"

float ballRadius = 0.2f;
unsigned int ballSegmants = 100;
float ballSpeed = 2.0f;
float ballMass = 1.0f;
float ballRestitution = 1.0f;
glm::vec2 ballAccel = g;