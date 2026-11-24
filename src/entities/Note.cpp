//
// Created by david on 11/6/2025.
//

#include "entities/Note.hpp"
#include "graphics/SfmlOutput.hpp"

Note::Note(const sf::Vector2f& position) {
    m_shape.setSize(sf::Vector2f(20.f, 20.f));
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setPosition(position);
}

void Note::draw(sf::RenderTarget& target) const {
    target.draw(m_shape);
}

sf::FloatRect Note::getBounds() const {
    return m_shape.getGlobalBounds();
}

std::ostream& operator<<(std::ostream& os, const Note& n) {
    os << "Note{bounds " << n.getBounds() << "}";
    return os;
}
