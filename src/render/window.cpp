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

  glViewport(0, 0, width, height);
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
