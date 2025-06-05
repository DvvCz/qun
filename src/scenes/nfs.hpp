#include <entt/entt.hpp>
#include <expected>

#include "render/renderer.hpp"

namespace scenes::nfs {
  std::expected<void, std::string> startup(entt::registry& reg, Renderer& renderer);
  std::expected<void, std::string> update(entt::registry& reg, Renderer& renderer);
}
