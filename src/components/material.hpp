#pragma once

#include <memory>

#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"

namespace components {
  using Material3D = std::shared_ptr<material::Block3D>;
  using Material2D = std::shared_ptr<material::Block2D>;
}
