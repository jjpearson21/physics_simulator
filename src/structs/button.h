#pragma once

#include <glm/glm.hpp>

#include <string>

struct Button
{
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
    std::string text;

    bool is_hovered(double mouseX, double mouseY)
    {
        return mouseX >= position.x &&
               mouseX <= position.x + size.x &&
               mouseY >= position.y &&
               mouseY <= position.y + size.y;
    }
};