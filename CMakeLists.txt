cmake_minimum_required(VERSION 3.20)
project(trender VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(glad STATIC ./thirdparty/glad/glad.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)

add_executable(trender
    ./src/main.cpp
    ./src/input/raw/keyboard.cpp
    ./src/input/raw/mouse.cpp
    ./src/render/window.cpp
    ./src/render/renderer.cpp
    ./src/shader/shader.cpp
    ./src/shader/program.cpp
    ./src/resource/obj/obj.cpp
)

target_include_directories(trender PRIVATE ./thirdparty)

target_link_libraries(trender glad glfw OpenGL::GL glm::glm)