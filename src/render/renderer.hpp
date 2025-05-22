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

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shaderProgram;
};