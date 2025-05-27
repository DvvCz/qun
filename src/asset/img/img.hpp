#pragma once

#include <expected>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <optional>

#include <stb/stb_image.h>
#include "../asset.hpp"

namespace asset {

  class Img : public Asset {
  public:
    [[nodiscard]] std::optional<std::string> getName() const noexcept {
      return "Img";
    };

    [[nodiscard]] static std::expected<Img, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    std::filesystem::path getPath() const noexcept {
      return path;
    }

    Img(int width, int height, int channels, std::vector<unsigned char> data, std::filesystem::path path)
        : width(width), height(height), channels(channels), data(std::move(data)), path(std::move(path)) {
    }

    [[nodiscard]] int getWidth() const noexcept {
      return width;
    }

    [[nodiscard]] int getHeight() const noexcept {
      return height;
    }

    [[nodiscard]] int getChannels() const noexcept {
      return channels;
    }

    [[nodiscard]] const std::vector<unsigned char>& getData() const noexcept {
      return data;
    }

  private:
    int width;
    int height;
    int channels;
    std::vector<unsigned char> data;
    std::filesystem::path path;
  };
}
