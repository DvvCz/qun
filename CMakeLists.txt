cmake_minimum_required(VERSION 3.20)
project(qun VERSION 0.1.0 LANGUAGES C CXX)

# Force static linking for all vcpkg libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

# Platform-specific static linking settings
if(WIN32)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .lib .a)
    set(BUILD_SHARED_LIBS OFF)
elseif(UNIX)
    set(CMAKE_FIND_LIBRARY_SUFFIXES .a .so)
    set(BUILD_SHARED_LIBS OFF)
endif()

# Force static linking for specific libraries
set(GLFW_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Stb REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(fastgltf CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# glad
add_library(glad STATIC ./thirdparty/glad/src/gl.c)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)
target_include_directories(glad PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include)


# rapidobj
add_library(rapidobj INTERFACE)
target_include_directories(rapidobj SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidobj/include)

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
    ./src/render/model/3d/icosphere.cpp
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

if (MSVC)
  target_link_options(qun PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif()

target_include_directories(qun
    PRIVATE
    ./thirdparty
    ./thirdparty/glad/include
    ./src
)

target_link_libraries(qun
    glad
    rapidobj
    EnTT::EnTT
    fastgltf::fastgltf
    glfw
    OpenGL::GL
    glm::glm
)
