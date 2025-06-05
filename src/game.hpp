#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>
#include <expected>
#include <entt/entt.hpp>

#include "render/renderer.hpp"
#include "render/window.hpp"
#include "resources/time.hpp"

enum class Schedule {
  Startup,
  Update,
  Render
};

class Game {
public:
  Game();

  std::expected<bool, std::string> start();

  template <typename T> T& getResource() {
    if constexpr (std::is_same_v<T, resources::Time>) {
      return time;
    } else if constexpr (std::is_same_v<T, entt::registry>) {
      return *registry;
    } else {
      static_assert(std::is_same_v<T, void>, "Resource type not registered");
    }
  }

  template <typename... Resources, typename Func> void addSystem(Schedule schedule, Func&& system) {
    scheduledSystems[schedule].emplace_back(
        [this, system = std::forward<Func>(system)]() { system(getResource<Resources>()...); });
  }

  void addDefaultSystems();

private:
  resources::Time time;
  std::unordered_map<Schedule, std::vector<std::function<void()>>> scheduledSystems;

  void runSchedule(Schedule schedule) {
    if (auto it = scheduledSystems.find(schedule); it != scheduledSystems.end()) {
      for (auto& system : it->second) {
        system();
      }
    }
  }

  // ECS
  std::shared_ptr<entt::registry> registry;
  // std::unique_ptr<systems::Particle> particleSystem;
  // std::unique_ptr<systems::Transform> transformSystem;

  // std::unique_ptr<systems::ParticleSpawner> particleSpawnerSystem;

  // Boids systems
  // std::unique_ptr<systems::Boids> boidsSystem;
  // std::unique_ptr<systems::BoidSpawner> boidSpawnerSystem;

  // Rendering
  std::shared_ptr<Window> window;
  std::unique_ptr<Renderer> renderer;
};
