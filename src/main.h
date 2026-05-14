#pragma once

// ===== Settings =====
// window
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;

// ===== App State =====
enum class AppState
{
    MainMenu,
    CollisionWithBalls,
    Paused
};