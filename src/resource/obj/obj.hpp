#pragma once

#include <expected>
#include <string>
#include <vector>

#include "../asset.hpp"

namespace resource {
  class Obj {
  public:
    Obj();

    [[nodiscard]] static std::expected<Asset, std::string> tryFromBuffer(const std::vector<std::byte>& buffer) noexcept;

  private:
    struct Vertex {
      float x;
      float y;
      float z;
      float w;
    };

    struct Normal {
      float x;
      float y;
      float z;
    };

    struct TexCoord {
      float u;
      float v;
      float w;
    };

    struct Face {
      int vertexIndices[3];
      int normalIndices[3];
      int texCoordIndices[3];
    };

    struct Line {
      std::vector<int> vertexIndices;
    };

    static std::expected<void, std::string> consumeComment(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static void consumeWhitespace(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;

    static std::expected<Vertex, std::string> consumeVertex(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static std::expected<Normal, std::string> consumeNormal(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static std::expected<TexCoord, std::string> consumeTexCoord(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;

    static std::expected<Face, std::string> consumeFace(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static std::expected<Line, std::string> consumeLine(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;

    static std::expected<float, std::string> consumeFloat(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static std::expected<int, std::string> consumeInt(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
    static std::expected<std::string, std::string> consumeIdent(size_t& ptr, const std::vector<std::byte>& buffer) noexcept;
  };
}