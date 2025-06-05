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

# entt
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/entt/single_include)

# fastgltf
add_subdirectory(thirdparty/fastgltf)

# TRender
add_executable(trender
    ./src/main.cpp
    ./src/game.cpp
    ./src/input/raw/keyboard.cpp
    ./src/input/raw/mouse.cpp
    ./src/render/window.cpp
    ./src/render/renderer.cpp
    ./src/render/model/2d/quad.cpp
    ./src/render/model/3d/cube.cpp
    ./src/render/model/3d/sphere.cpp
    ./src/render/model/3d/asset.cpp
    ./src/render/material/material2d.cpp
    ./src/render/material/material3d.cpp
    ./src/render/uniform/block.cpp
    ./src/render/texture.cpp
    ./src/render/shader/shader.cpp
    ./src/render/shader/program.cpp
    ./src/asset/obj/obj.cpp
    ./src/asset/img/img.cpp
    ./src/asset/gltf/convert.cpp
    ./src/asset/gltf/material.cpp
    ./src/asset/gltf/node.cpp
    ./src/asset/gltf/texture.cpp
    ./src/asset/gltf/gltf.cpp
    ./src/util/error.cpp
    ./src/systems/particle.cpp
    ./src/systems/transform.cpp
    ./src/systems/particle-spawner.cpp
    ./src/systems/boids.cpp
    ./src/systems/boid-spawner.cpp

    # Test Scene (included by default)
    # ./src/scenes/test.cpp

    # Need for Speed Scene (might not have assets included)
    # ./src/scenes/nfs.cpp
)

target_include_directories(trender
    PRIVATE
    ./thirdparty
    ./thirdparty/fastgltf/include
    ./src
)

target_link_libraries(trender
    glad
    rapidobj
    entt
    fastgltf
    glfw
    OpenGL::GL
    glm::glm
)
