#pragma once

#include "window.hpp"
#include "../shader/program.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

class Renderer final {
public:
  Renderer(const std::shared_ptr<Window>& window);

  void drawFrame() const;

private:
  std::shared_ptr<Window> window;
  shader::Program shaderProgram;
  uint32_t vao;
  uint32_t vbo;
};