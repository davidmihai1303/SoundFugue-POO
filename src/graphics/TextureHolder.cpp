//
// Created by david on 11/2/2025.
//

#include "graphics/TextureHolder.hpp"
#include "graphics/SfmlOutput.hpp"
#include <stdexcept>

TextureHolder::TextureHolder(const std::string& path) : m_texture(std::make_unique<sf::Texture>()) {
    // A holder without its texture would only draw invisible sprites, so it must not exist at all
    if (!(*m_texture).loadFromFile(path)) {
        throw std::runtime_error("Couldn't load texture at " + path);
    }
}

sf::Texture& TextureHolder::get() const {
    if (!m_texture) {
        throw std::runtime_error("Texture not loaded!");
    }
    return *m_texture;
}

std::ostream& operator<<(std::ostream& os, const TextureHolder& holder) {
    os << "TextureHolder{size " << (*holder.m_texture).getSize() << "}";
    return os;
}
