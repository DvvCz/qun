#pragma once

#include <entt/entt.hpp>
#include <expected>

#include "render/renderer.hpp"

namespace scenes::test {
  std::expected<void, std::string> startup(entt::registry& reg, Renderer& renderer);
  std::expected<void, std::string> update(entt::registry& reg, Renderer& renderer);
}
