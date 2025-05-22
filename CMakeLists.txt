cmake_minimum_required(VERSION 3.20)
project(trender VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Glad
add_library(glad STATIC ./thirdparty/glad/glad.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

# Rapidobj
add_library(rapidobj INTERFACE)
target_include_directories(rapidobj INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidobj/include)

# stb_image
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)

# TRender
add_executable(trender
    ./src/main.cpp
    ./src/input/raw/keyboard.cpp
    ./src/input/raw/mouse.cpp
    ./src/render/window.cpp
    ./src/render/renderer.cpp
    ./src/render/model.cpp
    ./src/render/uniform.cpp
    ./src/render/texture.cpp
    ./src/render/material.cpp
    ./src/shader/shader.cpp
    ./src/shader/program.cpp
    ./src/resource/obj/obj.cpp
    ./src/resource/img/img.cpp
)

target_include_directories(trender
    PRIVATE
    ./thirdparty
)

target_link_libraries(trender
    glad
    rapidobj
    glfw
    OpenGL::GL
    glm::glm
)
