#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <expected>
#include <tuple>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <entt/entt.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "plugins/input/input.hpp"

#include "plugins/transform/transform.hpp"
#include "plugins/render/render.hpp"
#include "plugins/time/time.hpp"
#include "plugins/entt/entt.hpp"

enum class Schedule {
  Startup,
  Update,
  PostUpdate,
  Render
};

class Game;

template <typename T>
concept Plugin = requires(T plugin, Game& game) { plugin.build(game); };

template <Plugin... Plugins> struct PluginGroup {
  std::tuple<Plugins...> plugins;

  PluginGroup(Plugins... plugins) : plugins(std::make_tuple(plugins...)) {
  }

  void build(Game& game) {
    std::apply([&game](auto&... plugin) { (plugin.build(game), ...); }, plugins);
  }
};

class Game {
public:
  std::expected<bool, std::string> start();
  void requestExit();

  template <Plugin T> void addPlugin(T plugin) {
    plugin.build(*this);
  }

  template <Plugin T, typename... Args> void addPlugin(Args&&... args) {
    T plugin(std::forward<Args>(args)...);
    plugin.build(*this);
  }

  template <typename Func> void addSystem(Schedule schedule, Func&& system) {
    addSystemImpl(schedule, std::forward<Func>(system));
  }

private:
  // SFINAE helper to detect function signatures and extract parameter types
  template <typename Func> void addSystemImpl(Schedule schedule, Func&& system) {
    if constexpr (std::is_function_v<std::remove_pointer_t<std::decay_t<Func>>>) {
      // Function pointer case
      addSystemFromSignature(schedule, std::forward<Func>(system), std::decay_t<Func>{});
    } else {
      // Lambda/functor case - use operator()
      addSystemFromSignature(schedule, std::forward<Func>(system), &Func::operator());
    }
  }

  // Specialization for function pointers
  template <typename Func, typename R, typename... Args>
  void addSystemFromSignature(Schedule schedule, Func&& system, R (*)(Args...)) {
    addSystemInternal<std::decay_t<Args>...>(schedule, std::forward<Func>(system));
  }

  // Specialization for const member functions (lambdas)
  template <typename Func, typename Class, typename R, typename... Args>
  void addSystemFromSignature(Schedule schedule, Func&& system, R (Class::*)(Args...) const) {
    addSystemInternal<std::decay_t<Args>...>(schedule, std::forward<Func>(system));
  }

  // Specialization for non-const member functions
  template <typename Func, typename Class, typename R, typename... Args>
  void addSystemFromSignature(Schedule schedule, Func&& system, R (Class::*)(Args...)) {
    addSystemInternal<std::decay_t<Args>...>(schedule, std::forward<Func>(system));
  }

  // The actual implementation
  template <typename... Resources, typename Func> void addSystemInternal(Schedule schedule, Func&& system) {
    scheduledSystems[schedule].emplace_back([this, system = std::forward<Func>(system)]() -> std::expected<void, std::string> {
      using ReturnType = std::invoke_result_t<Func, Resources&...>;

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

public:
  template <typename T> void addResource(T&& resource) {
    resources[std::type_index(typeid(T))] = std::make_any<T>(std::forward<T>(resource));
  }

  template <typename T> void addResource(const T& resource) {
    resources[std::type_index(typeid(T))] = std::make_any<T>(resource);
  }

private:
  bool isRunning;
  std::unordered_map<std::type_index, std::any> resources;
  std::unordered_map<Schedule, std::vector<std::function<std::expected<void, std::string>()>>> scheduledSystems;

  template <typename T> T& getResource() {
    auto it = resources.find(std::type_index(typeid(T)));
    if (it == resources.end()) {
      throw std::runtime_error("Resource type not registered: " + std::string(typeid(T).name()));
    }

    return std::any_cast<T&>(it->second);
  }

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
};

namespace plugins {
  struct EnTT;
  struct Transform;
  struct Time;
  struct Render;
};

class DefaultPlugins {
private:
  PluginGroup<plugins::EnTT, plugins::Transform, plugins::Time, plugins::Render, plugins::Input> pluginGroup;

public:
  DefaultPlugins() : pluginGroup(plugins::EnTT{}, plugins::Transform{}, plugins::Time{}, plugins::Render{}, plugins::Input{}) {
  }

  void build(Game& game) {
    pluginGroup.build(game);
  }
};
