#pragma once

#include <memory>

#include "../render/model/model.hpp"
#include "../render/material.hpp"

namespace components {
  using Model = std::shared_ptr<Model>;
  using Material = std::shared_ptr<MaterialBlock>;
}
