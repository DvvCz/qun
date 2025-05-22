#pragma once

#include "window.hpp"
#include "../shader/program.hpp"
#include "../resource/asset.hpp"
#include "uniform.hpp"
#include "model.hpp"
#include "sampler.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

#define MAX_LIGHTS 20

struct Light {
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 color;
};

struct LightBlock {
  GLuint lightCount;
  Light lights[MAX_LIGHTS];
};

class Renderer final {
public:
  Renderer(const std::shared_ptr<Window>& window);

  void drawFrame() const;
  void addModel(const resource::ObjAsset& asset) noexcept;
  void addTexture(const resource::ImgAsset& texture) noexcept;

private:
  std::shared_ptr<TextureManager> textureManager;
  std::vector<std::unique_ptr<AssetModel>> assetModels;

  Uniform<glm::mat4x4> uniformProjMatrix;
  Uniform<glm::mat4x4> uniformViewMatrix;
  Uniform<glm::mat4x4> uniformModelMatrix;

  Uniform<GLuint> uniformTextureArray;
  Uniform<GLint> uniformTextureIdx;
  UniformBlock<LightBlock> uniformLightBlock;

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  LightBlock lightBlock;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shaderProgram;
};