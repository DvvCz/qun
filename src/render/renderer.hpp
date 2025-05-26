#pragma once

#include "window.hpp"
#include "../shader/program.hpp"
#include "uniform.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "model/asset.hpp"

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

  [[nodiscard]] std::shared_ptr<AssetModel> createAssetModel(const resource::ObjAsset& asset) const;

private:
  std::shared_ptr<TextureManager> textureManager;
  std::shared_ptr<MaterialManager> materialManager;
  std::shared_ptr<entt::registry> registry;

  Uniform<glm::mat4x4> uniformProjMatrix;
  Uniform<glm::mat4x4> uniformViewMatrix;
  Uniform<glm::mat4x4> uniformModelMatrix;

  Uniform<GLuint> uniformTextureArray;
  Uniform<GLint> uniformTextureIdx;

  Uniform<glm::vec3> uniformCameraPos;

  UniformBlock<LightBlock> uniformLightBlock;
  UniformBlock<MaterialBlock> uniformMaterialBlock;

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  LightBlock lightBlock;

  glm::vec3 cameraPos;
  glm::vec3 cameraFront;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shaderProgram;
};
