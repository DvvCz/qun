#include <entt/entt.hpp>
#include <expected>

class Game;

namespace scenes::nfs {
  namespace components {
    struct CameraState {};
  };

  struct NFS {
    void build(Game& game);
  };
};
