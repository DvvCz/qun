cmake_minimum_required(VERSION 3.20)
project(trender VERSION 0.1.0 LANGUAGES CXX)

find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(trender
    ./src/main.cpp
    ./src/input/raw/keyboard.cpp
    ./src/input/raw/mouse.cpp
    ./src/render/window.cpp
    ./src/render/renderer.cpp
)

target_include_directories(trender PRIVATE ./thirdparty)

target_link_libraries(trender glfw OpenGL::GL)

# Define OpenGL version 4.6
target_compile_definitions(trender PRIVATE GL_VERSION_MAJOR=4 GL_VERSION_MINOR=6)