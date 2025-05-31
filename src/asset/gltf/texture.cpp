#include "gltf.hpp"

#include "asset/img/img.hpp"

/* clang-format off */
std::expected<size_t, std::string> asset::loader::Gltf::tryCreateTexture(
  const fastgltf::Asset& asset,
  const fastgltf::Image& image,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  if (std::holds_alternative<fastgltf::sources::URI>(image.data)) {
    auto& uriData = std::get<fastgltf::sources::URI>(image.data);
    return std::unexpected("todo: uri texture loading not implemented yet");
  }

  if (std::holds_alternative<fastgltf::sources::BufferView>(image.data)) {
    auto& bufferData = std::get<fastgltf::sources::BufferView>(image.data);
    auto& bufferView = asset.bufferViews[bufferData.bufferViewIndex];

    auto& buffer = asset.buffers[bufferView.bufferIndex];
    if (!std::holds_alternative<fastgltf::sources::Array>(buffer.data)) {
      return std::unexpected("Buffer data is not an array, cannot load texture");
    }

    auto& arrayData = std::get<fastgltf::sources::Array>(buffer.data);

    switch (bufferData.mimeType) {
    case fastgltf::MimeType::PNG:
    case fastgltf::MimeType::JPEG: {
      auto dataStart = arrayData.bytes.cbegin() + bufferView.byteOffset;
      auto dataEnd = dataStart + bufferView.byteLength;
      std::vector<std::byte> imageData(dataStart, dataEnd);

      auto out = asset::loader::Img::tryFromData(imageData, texMan);
      if (!out.has_value()) {
        return std::unexpected{out.error()};
      }

      return out.value().textureId;
    }
    default:
      return std::unexpected{
          std::format("Unsupported texture mime type: {}", fastgltf::getMimeTypeString(bufferData.mimeType))};
    }
  }

  if (std::holds_alternative<fastgltf::sources::Array>(image.data)) {
    auto& arrayData = std::get<fastgltf::sources::Array>(image.data);

    std::vector<std::byte> imageData(arrayData.bytes.cbegin(), arrayData.bytes.cend());

    auto out = asset::loader::Img::tryFromData(imageData, texMan);
    if (!out.has_value()) {
      return std::unexpected{out.error()};
    }

    return out.value().textureId;
  }

  return std::unexpected{"Unknown buffer data type for texture"};
}
