//
// Created by david on 11/2/2025.
//

#ifndef SOUNDFUGUE_TEXTUREHOLDER_HPP
#define SOUNDFUGUE_TEXTUREHOLDER_HPP
#include <SFML/Graphics.hpp>
#include <memory>
#include <ostream>
#include <string>

class TextureHolder {
public:
    friend std::ostream& operator<<(std::ostream& os, const TextureHolder& holder);

    explicit TextureHolder(const std::string& path);

    // Forbid copying textures. Only copy by reference
    TextureHolder(const TextureHolder&) = delete;

    TextureHolder& operator=(const TextureHolder&) = delete;

    // We use reference to avoid making a copy of the large texture object
    [[nodiscard]] sf::Texture& get() const;

private:
    std::unique_ptr<sf::Texture> m_texture;
};

#endif //SOUNDFUGUE_TEXTUREHOLDER_HPP
