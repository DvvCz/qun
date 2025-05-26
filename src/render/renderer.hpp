#pragma once

#include "window.hpp"
#include "shader/program.hpp"

#include "render/uniform.hpp"
#include "render/texture.hpp"
#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"
#include "render/model/3d/asset.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#define MAX_LIGHTS 20

struct Light {
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 color;
  float radius;
};

struct LightBlock {
  GLuint lightCount;
  Light lights[MAX_LIGHTS];
};

class Renderer final {
public:
  Renderer(const std::shared_ptr<Window>& window, const std::shared_ptr<entt::registry>& registry);

  // 16:9 aspect ratio constant
  static constexpr float ASPECT_RATIO = 16.0f / 9.0f;
  const glm::vec3 upDir = glm::vec3(0.0f, 0.0f, 1.0f);

  void drawFrame();

  void setProjectionMatrix(const glm::mat4x4& projMatrix) noexcept;
  void setViewMatrix(const glm::mat4x4& viewMatrix) noexcept;
  void setModelMatrix(const glm::mat4x4& modelMatrix) noexcept;

  void setCameraPos(const glm::vec3& cameraPos) noexcept;
  void setCameraDir(const glm::vec3& cameraDir) noexcept;

  [[nodiscard]] const glm::mat4x4& getProjectionMatrix() const noexcept;
  [[nodiscard]] const glm::mat4x4& getViewMatrix() const noexcept;
  [[nodiscard]] const glm::mat4x4& getModelMatrix() const noexcept;

  [[nodiscard]] const glm::vec3& getCameraPos() const noexcept;

  [[nodiscard]] std::shared_ptr<model::Asset> createAsset3D(const resource::ObjAsset& asset) const;

private: // todo: this is a mess, separate 2d and 3d into structs
  std::shared_ptr<TextureManager> textureManager2D;
  std::shared_ptr<TextureManager> textureManager3D;

  std::shared_ptr<material::Manager2D> materialManager2D;
  std::shared_ptr<material::Manager3D> materialManager3D;

  std::shared_ptr<entt::registry> registry;

  // 3d uniforms
  Uniform<glm::mat4x4> uniformProjMatrix3D;
  Uniform<glm::mat4x4> uniformViewMatrix3D;
  Uniform<glm::mat4x4> uniformModelMatrix3D;
  Uniform<GLuint> uniformTextureArray3D;
  Uniform<GLint> uniformTextureIdx3D;
  Uniform<glm::vec3> uniformCameraPos3D;
  UniformBlock<LightBlock> uniformLightBlock3D;
  UniformBlock<material::Block3D> uniformMaterialBlock3D;

  // 2d uniforms
  Uniform<GLuint> uniformTextureArray2D;
  Uniform<GLint> uniformTextureIdx2D;
  UniformBlock<material::Block2D> uniformMaterialBlock2D;

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  LightBlock lightBlock;

  glm::vec3 cameraPos;
  glm::vec3 cameraFront;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shader3D;
  std::unique_ptr<shader::Program> shader2D;
};
