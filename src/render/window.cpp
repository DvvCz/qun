#include "window.hpp"

Window::Window(uint16_t width, uint16_t height, std::string title)
    : currentWidth(width), currentHeight(height), currentTitle(title) {

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(glfwWindow, this);
  glfwMakeContextCurrent(glfwWindow);
  glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetFramebufferSizeCallback(glfwWindow, onResize); /* clang-format on */
}

Window::~Window() {
  glfwDestroyWindow(glfwWindow);
}

void Window::onResize(GLFWwindow* window, int width, int height) {
  auto* wrappedWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  wrappedWindow->currentWidth = static_cast<uint16_t>(width);
  wrappedWindow->currentHeight = static_cast<uint16_t>(height);

  // Calculate viewport to maintain 16:9 aspect ratio
  // Using the same constant as defined in Renderer
  constexpr float targetAspectRatio = 16.0f / 9.0f;

  float currentAspectRatio = static_cast<float>(width) / static_cast<float>(height);

  int viewportX = 0;
  int viewportY = 0;
  int viewportWidth = width;
  int viewportHeight = height;

  // If current aspect ratio is wider than target, apply pillarboxing (black bars on sides)
  if (currentAspectRatio > targetAspectRatio) {
    viewportWidth = static_cast<int>(height * targetAspectRatio);
    viewportX = (width - viewportWidth) / 2;
  }
  // If current aspect ratio is taller than target, apply letterboxing (black bars on top/bottom)
  else if (currentAspectRatio < targetAspectRatio) {
    viewportHeight = static_cast<int>(width / targetAspectRatio);
    viewportY = (height - viewportHeight) / 2;
  }

  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(glfwWindow) == GL_TRUE;
}

GLFWwindow* Window::getGlfwWindow() const {
  return glfwWindow;
}

uint16_t Window::getWidth() const {
  return currentWidth;
}

uint16_t Window::getHeight() const {
  return currentHeight;
}
