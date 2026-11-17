#ifndef SOUNDFUGUE_TERRAINMAPLOADER_HPP
#define SOUNDFUGUE_TERRAINMAPLOADER_HPP

#include <SFML/Graphics/Rect.hpp>
#include <tmxlite/Map.hpp>
#include <vector>

// Converts the terrain objects of an already parsed Tiled map into world-space rectangles for TerrainCollision.
// Reports an invalid map by throwing std::invalid_argument.
[[nodiscard]] std::vector<sf::FloatRect> loadTerrainRectangles(const tmx::Map& map);

#endif // SOUNDFUGUE_TERRAINMAPLOADER_HPP
