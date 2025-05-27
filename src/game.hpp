#pragma once

#include <memory>
#include <entt/entt.hpp>

#include "render/renderer.hpp"
#include "render/window.hpp"

#include "asset/gltf/gltf.hpp"

class Game {
public:
  Game();

  std::expected<bool, std::string> start();

private:
  std::expected<bool, std::string> setupScene();

  std::shared_ptr<Window> window;
  std::shared_ptr<entt::registry> registry;
  std::unique_ptr<Renderer> renderer;

  asset::loader::Gltf gltfLoader;
};
