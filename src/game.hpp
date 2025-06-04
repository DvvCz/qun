#pragma once

#include <memory>
#include <entt/entt.hpp>

#include "render/renderer.hpp"
#include "render/window.hpp"

#include "systems/particle-spawner.hpp"
#include "systems/particle.hpp"
#include "systems/transform.hpp"

class Game {
public:
  Game();

  std::expected<bool, std::string> start();

private:
  std::expected<bool, std::string> setupScene() noexcept;
  void createSystems() noexcept;

  // ECS
  std::shared_ptr<entt::registry> registry;
  std::unique_ptr<systems::Particle> particleSystem;
  std::unique_ptr<systems::Transform> transformSystem;

  std::unique_ptr<systems::ParticleSpawner> particleSpawnerSystem;

  // Rendering
  std::shared_ptr<Window> window;
  std::unique_ptr<Renderer> renderer;
};
