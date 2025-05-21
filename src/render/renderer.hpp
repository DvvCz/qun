#pragma once

#include "window.hpp"
#include "../shader/program.hpp"
#include "../resource/asset.hpp"
#include "uniform.hpp"
#include "model.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>

class Renderer final {
public:
  Renderer(const std::shared_ptr<Window>& window);

  void drawFrame() const;
  void addModel(const resource::ObjAsset& asset);

private:
  TriangleModel triangleModel;
  std::vector<std::unique_ptr<AssetModel>> assetModels;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shaderProgram;
};