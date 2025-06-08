#pragma once

#include <entt/entt.hpp>

namespace components {
  struct Parent {
    std::vector<entt::entity> children;
  };

  struct Child {
    entt::entity parent;
  };
};
