#pragma once

#include <memory>

#include "../render/model/model.hpp"
#include "../render/material.hpp"

namespace components {
  using Model3D = std::shared_ptr<Model3D>;
  using Model2D = std::shared_ptr<Model2D>;
  using Material = std::shared_ptr<MaterialBlock>;
}
