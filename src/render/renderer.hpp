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

  void updateObjects();
  std::vector<ObjectAsset> objects;

private:
  void addUniform(const Uniform& uniform) const;
  void useShader(const shader::Program& program) const;

  void drawObject(const ObjectAsset& object) const;

  TriangleModel triangleModel;

  std::shared_ptr<Window> window;
  std::unique_ptr<shader::Program> shaderProgram;
};