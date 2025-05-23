#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <cstdint>
#include <glm/glm.hpp>

namespace Input {
  enum class MouseButton : uint16_t {
    Button1 = GLFW_MOUSE_BUTTON_1,
    Button2 = GLFW_MOUSE_BUTTON_2,
    Button3 = GLFW_MOUSE_BUTTON_3,
    Button4 = GLFW_MOUSE_BUTTON_4,
    Button5 = GLFW_MOUSE_BUTTON_5,
    Button6 = GLFW_MOUSE_BUTTON_6,
    Button7 = GLFW_MOUSE_BUTTON_7,
    Button8 = GLFW_MOUSE_BUTTON_8,
    Last = GLFW_MOUSE_BUTTON_LAST,
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE
  };

  class Mouse final {
  public:
    static void bindGlfwCallbacks(GLFWwindow* const glfwWindow);

    [[nodiscard]] static bool wasJustPressed(const MouseButton button);
    [[nodiscard]] static bool wasJustReleased(const MouseButton button);
    [[nodiscard]] static bool isCurrentlyHeld(const MouseButton button);

    [[nodiscard]] static glm::vec2 getPosition();
    [[nodiscard]] static glm::vec2 getPositionDelta();

    static void resetCurrentMouseMaps();

  private:
    using MouseButtonMap = std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1>;

    static MouseButtonMap mouseButtonsJustPressed;
    static MouseButtonMap mouseButtonsJustReleased;
    static MouseButtonMap mouseButtonsCurrentlyHeld;

    static glm::vec2 mousePosition;
    static glm::vec2 mousePositionLastFrame;

    static void onMouseButtonPressed(const MouseButton button);
    static void onMouseButtonReleased(const MouseButton button);

    static void onGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void onGlfwMouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
  };
}
