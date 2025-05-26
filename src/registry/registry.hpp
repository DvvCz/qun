#pragma once

#include "entity.hpp"
#include <vector>
#include <memory>

class Registry {
public:
  Registry();

  std::shared_ptr<Entity> createEntity();

private:
  std::vector<std::shared_ptr<Entity>> entities;
};
