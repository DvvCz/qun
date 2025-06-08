cmake_minimum_required(VERSION 3.20)
project(qun VERSION 0.1.0 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Glad
add_library(glad STATIC ./thirdparty/glad/src/gl.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include)

# Rapidobj
add_library(rapidobj INTERFACE)
target_include_directories(rapidobj SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidobj/include)

# stb_image
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/stb)

# entt
add_library(entt INTERFACE)
target_include_directories(entt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/entt/single_include)

# fastgltf
add_subdirectory(thirdparty/fastgltf)

# qun
add_executable(qun
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
    ./src/plugins/render/render.cpp
    ./src/plugins/time/time.cpp
    ./src/plugins/transform/transform.cpp
    ./src/plugins/entt/entt.cpp
    ./src/plugins/input/input.cpp
    ./src/plugins/debug-cam-controller/debug-cam-controller.cpp
    ./src/plugins/physics/physics.cpp

    # Test Scene (included by default)
    ./src/scenes/test.cpp

    # Need for Speed Scene (might not have assets included)
    ./src/scenes/nfs.cpp
)

target_include_directories(qun
    PRIVATE
    ./thirdparty
    ./thirdparty/fastgltf/include
    ./thirdparty/glad/include
    ./src
)

target_link_libraries(qun
    glad
    rapidobj
    entt
    fastgltf
    glfw
    OpenGL::GL
    glm::glm
)
