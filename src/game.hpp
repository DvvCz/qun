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
    } else if constexpr (std::is_same_v<T, Renderer>) {
      return *renderer;
    } else {
      static_assert(std::is_same_v<T, void>, "Resource type not registered");
    }
  }

  template <typename... Resources, typename Func> void addSystem(Schedule schedule, Func&& system) {
    scheduledSystems[schedule].emplace_back([this, system = std::forward<Func>(system)]() -> std::expected<void, std::string> {
      using ReturnType = std::invoke_result_t<Func, decltype(getResource<Resources>())...>;

      if constexpr (std::is_same_v<ReturnType, void>) {
        system(getResource<Resources>()...);
        return {};
      } else if constexpr (std::is_same_v<ReturnType, std::expected<void, std::string>>) {
        return system(getResource<Resources>()...);
      } else {
        static_assert(/* clang-format off */
          std::is_same_v<ReturnType, void> ||
          std::is_same_v<ReturnType, std::expected<void, std::string>>,
          "System functions must return either void or std::expected<void, std::string>"
        ); /* clang-format on */
      }
    });
  }

  void addDefaultSystems();
  void addDefaultCameraController();

private:
  resources::Time time;
  std::unordered_map<Schedule, std::vector<std::function<std::expected<void, std::string>()>>> scheduledSystems;

  std::expected<void, std::string> runSchedule(Schedule schedule) {
    if (auto it = scheduledSystems.find(schedule); it != scheduledSystems.end()) {
      for (auto& system : it->second) {
        auto result = system();
        if (!result) {
          return std::unexpected(result.error());
        }
      }
    }
    return {};
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
