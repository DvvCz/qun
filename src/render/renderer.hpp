#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "window.hpp"

#include "render/uniform/single.hpp"
#include "render/uniform/block.hpp"
#include "render/texture.hpp"
#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"
#include "render/model/3d/asset.hpp"
#include "render/shader/program.hpp"

#define MAX_LIGHTS 20

namespace render {
  struct Light {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    float radius;
  };

  struct LightsArray {
    GLuint lightCount;
    Light lights[MAX_LIGHTS];
  };
}

class Renderer final {
public:
  Renderer(const std::shared_ptr<Window>& window, const std::shared_ptr<entt::registry>& registry);

  // 16:9 aspect ratio constant
  static constexpr float ASPECT_RATIO = 16.0f / 9.0f;

  void drawFrame();

  void setCameraPos(const glm::vec3& cameraPos) noexcept;
  void setCameraDir(const glm::vec3& cameraDir) noexcept;

  [[nodiscard]] const glm::vec3& getCameraPos() const noexcept;

  [[nodiscard]] std::shared_ptr<model::Asset> createAsset3D(const asset::Asset3D& asset) const;

  std::shared_ptr<texture::Manager> textureManager2D;
  std::shared_ptr<texture::Manager> textureManager3D;

private:
  void draw3D();
  void draw2D();

  // todo: this is a mess, separate 2d and 3d into structs
  std::shared_ptr<material::Manager2D> materialManager2D;
  std::shared_ptr<material::Manager3D> materialManager3D;

  std::shared_ptr<entt::registry> registry;

  // 3d uniforms
  uniform::Single<glm::mat4x4> uniformProjMatrix3D;
  uniform::Single<glm::mat4x4> uniformViewMatrix3D;
  uniform::Single<glm::mat4x4> uniformModelMatrix3D;
  uniform::Single<GLint> uniformTextureArray3D;
  uniform::Single<glm::vec3> uniformCameraPos3D;
  uniform::Block<render::LightsArray> uniformLightsArray3D;
  uniform::Block<material::Material3D> uniformMaterial3D;

  // 2d uniforms
  uniform::Single<GLint> uniformTextureArray2D;
  uniform::Block<material::Material2D> uniformMaterial2D;

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  render::LightsArray lightsArray;

  glm::vec3 cameraPos;
  glm::vec3 cameraFront;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shader3D;
  std::unique_ptr<shader::Program> shader2D;
};
