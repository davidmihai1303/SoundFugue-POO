#include "terrain/TerrainMapLoader.hpp"

#include <cmath>
#include <stdexcept>
#include <string>
#include <tmxlite/LayerGroup.hpp>
#include <tmxlite/ObjectGroup.hpp>

namespace {
const std::string collisionLayerName = "Collision";

// Terrain must come from a top-level layer, so a Collision layer inside any group, at any depth, is an error.
// NOLINTNEXTLINE(misc-no-recursion): depth equals the map's group nesting, which is small
void rejectNestedCollisionLayers(const std::vector<tmx::Layer::Ptr>& layers, const std::string& groupName) {
    for (const tmx::Layer::Ptr& layer : layers) {
        if ((*layer).getName() == collisionLayerName) {
            throw std::invalid_argument("Tiled map: layer '" + collisionLayerName + "' is inside group '" + groupName + "'; it must be a top-level layer");
        }
        if ((*layer).getType() == tmx::Layer::Type::Group) {
            rejectNestedCollisionLayers((*layer).getLayerAs<tmx::LayerGroup>().getLayers(), (*layer).getName());
        }
    }
}

// Returns the single top-level object layer named Collision.
const tmx::ObjectGroup& findCollisionLayer(const tmx::Map& map) {
    const tmx::ObjectGroup* collisionLayer = nullptr;

    for (const tmx::Layer::Ptr& layer : map.getLayers()) {
        if ((*layer).getType() == tmx::Layer::Type::Group) {
            rejectNestedCollisionLayers((*layer).getLayerAs<tmx::LayerGroup>().getLayers(), (*layer).getName());
        }
        if ((*layer).getName() != collisionLayerName) {
            continue;
        }
        if ((*layer).getType() != tmx::Layer::Type::Object) {
            throw std::invalid_argument("Tiled map: layer '" + collisionLayerName + "' must be an object layer");
        }
        if (collisionLayer != nullptr) {
            throw std::invalid_argument("Tiled map: more than one top-level layer is named '" + collisionLayerName + "'");
        }
        collisionLayer = &(*layer).getLayerAs<tmx::ObjectGroup>();
    }

    if (collisionLayer == nullptr) {
        throw std::invalid_argument("Tiled map: no top-level object layer named '" + collisionLayerName + "'");
    }
    return *collisionLayer;
}

// Names the object the way it can be found in Tiled: its layer, its unique ID, and its name when it has one.
std::string describeObject(const tmx::Object& object) {
    std::string description = "Tiled map: layer '" + collisionLayerName + "', object " + std::to_string(object.getUID());
    if (!object.getName().empty()) {
        description += " ('" + object.getName() + "')";
    }
    return description;
}

// Only ordinary, unrotated rectangles with finite geometry and positive size can become terrain.
void validateTerrainObject(const tmx::Object& object) {
    if (object.getShape() != tmx::Object::Shape::Rectangle) {
        throw std::invalid_argument(describeObject(object) + ": object is not a rectangle");
    }
    // Tile objects also report a rectangle shape; only their tile ID tells them apart.
    if (object.getTileID() != 0) {
        throw std::invalid_argument(describeObject(object) + ": object is a tile object, not a rectangle");
    }
    if (object.getRotation() != 0.f) {
        throw std::invalid_argument(describeObject(object) + ": rectangle is rotated");
    }
    const tmx::FloatRect& bounds = object.getAABB();
    if (!std::isfinite(bounds.left) || !std::isfinite(bounds.top) || !std::isfinite(bounds.width) || !std::isfinite(bounds.height)) {
        throw std::invalid_argument(describeObject(object) + ": rectangle has a non-finite position or size");
    }
    if (bounds.width <= 0.f || bounds.height <= 0.f) {
        throw std::invalid_argument(describeObject(object) + ": rectangle must have a positive width and height");
    }
}
} // namespace

std::vector<sf::FloatRect> loadTerrainRectangles(const tmx::Map& map) {
    const tmx::ObjectGroup& collisionLayer = findCollisionLayer(map);

    // Object bounds are relative to their layer. The layer offset is applied here, once, to reach world pixels.
    // tmxlite reads offsets as whole pixels.
    const tmx::Vector2i& layerOffset = collisionLayer.getOffset();
    const sf::Vector2f offset{static_cast<float>(layerOffset.x), static_cast<float>(layerOffset.y)};

    std::vector<sf::FloatRect> rectangles;
    rectangles.reserve(collisionLayer.getObjects().size());
    for (const tmx::Object& object : collisionLayer.getObjects()) {
        validateTerrainObject(object);
        const tmx::FloatRect& bounds = object.getAABB();
        rectangles.push_back({{bounds.left + offset.x, bounds.top + offset.y}, {bounds.width, bounds.height}});
    }
    return rectangles;
}
