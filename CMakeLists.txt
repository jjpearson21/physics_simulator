cmake_minimum_required(VERSION 3.20)
project(physics_simulator LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(physics_simulator
    src/main.cpp
    src/core/glad.c
    src/menus/main_menu.cpp
    src/physics/collision.cpp
    src/simulations/collisions_with_balls.cpp
)

target_include_directories(physics_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include
)

# Your project currently includes a Windows MSVC-style GLFW library: dependencies/lib/glfw3.lib
# This works best with the MSVC compiler.
target_link_libraries(physics_simulator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/lib/glfw3.lib
    opengl32
    user32
    gdi32
    shell32
)

# Make the exe run from the project root so relative shader/resource paths work.
set_target_properties(physics_simulator PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
