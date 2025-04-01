#include "renderer.hpp"

#define GL_GLEXT_PROTOTYPES

Renderer::Renderer(const std::shared_ptr<Window>& window) : window(window) {
}

void Renderer::drawFrame() const {
  float triangles[] = {/* clang-format off */
    -0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
  }; /* clang-format on */

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}