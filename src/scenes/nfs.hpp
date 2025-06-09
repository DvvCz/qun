#include <entt/entt.hpp>
#include <expected>

class Game;

namespace scenes::nfs {
  namespace components {
    struct Car {};

    struct CameraState {
      float yaw = 0.0f;       // Horizontal rotation around the car
      float pitch = -0.1f;    // Vertical angle (looking down slightly at the car)
      float distance = 10.0f; // Distance from the car
      float height = 3.0f;    // Height offset

      float mouseSensitivity = 0.18f;  // Mouse sensitivity (adjusted for delta time)
      float autoReturnSpeed = 2.0f;    // Speed of auto-centering
      float timeSinceLastInput = 0.0f; // Time since last mouse input
      float autoReturnDelay = 2.0f;    // Delay before auto-centering starts

      bool isUserControlling = false; // Whether user is actively controlling camera

      // Target values for smooth auto-centering
      float targetYaw = 0.0f;    // Target yaw (behind the car)
      float targetPitch = -0.1f; // Target pitch (slightly down)
    };
  };

  struct NFS {
    void build(Game& game);
  };
};
