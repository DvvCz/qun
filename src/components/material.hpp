#pragma once

#include <memory>

#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"

#include "asset/asset.hpp"

namespace components {
  using Material3D = std::shared_ptr<asset::Material>;
  using Material2D = std::shared_ptr<material::Material2D>;
}
