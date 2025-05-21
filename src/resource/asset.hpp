#pragma once

#include <vector>
#include <optional>
#include <string>
#include <array>

class Asset {
public:
  [[nodiscard]] virtual std::optional<std::string> getName() const noexcept = 0;
};