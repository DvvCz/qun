#include "asset/img/img.hpp"
#include "components/material.hpp"
#include "game.hpp"

#include <expected>
#include <memory>
#include <print>
#include <string>

#include <glm/glm.hpp>

#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"

#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"

#include "render/model/3d/sphere.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

std::expected<bool, std::string> Game::setupScene() noexcept {
  std::println("Loading assets, this might take a while...");

  { // gltf fish
    auto asset = asset::loader::Gltf::tryFromFile("resources/BarramundiFish.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load fish asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(5.0f));
    matrix = glm::translate(matrix, glm::vec3(0.0f, -0.25f, 0.1f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 5.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Light>(ent, glm::vec3(1.0f), 2.0f, 1000.0f);
  }

  auto greenMaterial = std::make_shared<asset::Material>();
  greenMaterial->ambient = glm::vec3(0.05f);
  greenMaterial->diffuse = glm::vec3(0.2f, 0.6f, 0.2f);
  greenMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  greenMaterial->shininess = 64.0f;
  greenMaterial->dissolve = 1.0f;

  auto blueMaterial = std::make_shared<asset::Material>();
  blueMaterial->ambient = glm::vec3(0.05f);
  blueMaterial->diffuse = glm::vec3(0.2f, 0.2f, 0.6f);
  blueMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  blueMaterial->shininess = 64.0f;
  blueMaterial->dissolve = 1.0f;

  auto metallicRedMaterial = std::make_shared<asset::Material>();
  metallicRedMaterial->ambient = glm::vec3(0.05f);
  metallicRedMaterial->diffuse = glm::vec3(0.8f, 0.1f, 0.1f);
  metallicRedMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  metallicRedMaterial->shininess = 128.0f; // Higher shininess for metallic look
  metallicRedMaterial->dissolve = 1.0f;

  { // bunny
    auto asset = asset::loader::Obj::tryFromFile("resources/Bunny.obj", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load bunny asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 2.0f, -0.325f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // blue cube
    auto model = std::make_shared<model::Cube>(glm::vec3(1.0f));

    auto matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 0.5f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, blueMaterial);
  }

  { // red sphere
    auto model = std::make_shared<model::Sphere>(0.5f, 4);

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(3.0f, 0.0f, 0.5f)); // Position it to the right of the cube

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, metallicRedMaterial); // Use the red metallic material
  }

  { // flight helmet
    auto asset = asset::loader::Gltf::tryFromFile("resources/FlightHelmet/FlightHelmet.gltf", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load helmet asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.5f));
    matrix = glm::translate(matrix, glm::vec3(-2.0f, 0.0f, 0.5f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // dragon
    auto asset = asset::loader::Gltf::tryFromFile("resources/DragonAttenuation.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load dragon asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.35f));
    matrix = glm::translate(matrix, glm::vec3(18.0f, 5.0f, 1.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  // { // car concept
  //   auto asset = asset::loader::Gltf::tryFromFile("resources/CarConcept/CarConcept.gltf", *renderer->textureManager3D);
  //   if (!asset.has_value()) {
  //     return std::unexpected{std::format("Failed to load car asset: {}", util::error::indent(asset.error()))};
  //   }

  //   auto model = renderer->createAsset3D(asset.value());

  //   auto matrix = glm::mat4(1.0f);
  //   matrix = glm::scale(matrix, glm::vec3(0.5f));
  //   matrix = glm::translate(matrix, glm::vec3(18.0f, 5.0f, 1.2f));
  //   matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to face forward

  //   auto ent = registry->create();
  //   registry->emplace<components::GlobalTransform>(ent, matrix);
  //   registry->emplace<components::Model3D>(ent, model);
  // }

  { // normal tangent test
    auto asset = asset::loader::Gltf::tryFromFile("resources/NormalTangentTest.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load normal tangent asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(-10.0f, 3.0f, 1.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // negative scale test
    auto asset = asset::loader::Gltf::tryFromFile("resources/NegativeScaleTest.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load negative scale asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.3f));
    matrix = glm::translate(matrix, glm::vec3(10.0f, 3.0f, 1.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // texture transform test
    auto asset = asset::loader::Gltf::tryFromFile("resources/TextureTransformTest/TextureTransformTest.gltf",
                                                  *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load transform asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(-10.0f, -5.0f, 1.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // skybox
    auto asset = asset::loader::Img::tryFromFile("resources/mountain.jpg", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load skybox asset: {}", util::error::indent(asset.error()))};
    }

    auto material = std::make_shared<asset::Material>();
    material->ambient = glm::vec3(1.0f);
    material->diffuse = glm::vec3(0.0f);
    material->specular = glm::vec3(0.0f);
    material->shininess = 0.0f;
    material->diffuseTexture = asset->texture;

    auto model = std::make_shared<model::Sphere>(100.0f, 4);

    auto matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, material);
  }

  { // baseplate
    auto model = std::make_shared<model::Cube>(glm::vec3(1000.0f, 1000.0f, 0.01f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, glm::identity<glm::mat4x4>());
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, greenMaterial);
  }

  // /* clang-format off */
  // auto basequad = std::make_shared<model::Quad>(
  //     Vertex2D{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},
  //     Vertex2D{glm::vec3(0.0f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
  //     Vertex2D{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
  //     Vertex2D{glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
  // ); /* clang-format on */
  // auto quadEnt = registry->create();
  // registry->emplace<components::GlobalTransform>(quadEnt, glm::mat4(1.0f));
  // registry->emplace<components::Model2D>(quadEnt, basequad);

  // auto blueMaterial = std::make_shared<material::Material2D>();
  // blueMaterial->color = glm::vec3(0.0f, 0.0f, 1.0f);
  // registry->emplace<components::Material2D>(quadEnt, blueMaterial);

  return true;
}
