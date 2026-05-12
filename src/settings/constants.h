#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// === MATH ===
// Approximate value of Pi
constexpr float PI = 3.14159265359f;

// === FORCES ===
// Earth's gravitational constant (m/s^2)
constexpr glm::vec2 g = glm::vec2(0.0f, -9.8f);