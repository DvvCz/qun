#pragma once

#include <entt/entt.hpp>
#include <expected>

#include "render/renderer.hpp"

namespace scenes::test {
  std::expected<void, std::string> startup(std::shared_ptr<entt::registry> reg, std::shared_ptr<Renderer> renderer);
}
