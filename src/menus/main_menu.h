#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "main.h"
#include "glm/glm.hpp"
#include "core/shader.h"
#include "structs/button.h"

// Function Prototypes
void setup_buffer_main_menu(unsigned int &mainMenuVAO, unsigned int &mainMenuVBO);
void render_button(Button button, Shader &mainMenuShader, unsigned int mainMenuVAO);
void render_main_menu(Shader &mainMenuShader, float aspect, unsigned int mainMenuVAO);
void update_main_menu(GLFWwindow *window, AppState &currentState);