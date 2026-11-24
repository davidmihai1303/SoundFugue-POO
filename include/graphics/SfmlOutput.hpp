#ifndef SOUNDFUGUE_SFMLOUTPUT_HPP
#define SOUNDFUGUE_SFMLOUTPUT_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <ostream>

// SFML has no stream output for its vector and rectangle types. These let every class print its geometry the same way.

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& vector) {
    os << "(" << vector.x << ", " << vector.y << ")";
    return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& rect) {
    os << "[position " << rect.position << ", size " << rect.size << "]";
    return os;
}

#endif // SOUNDFUGUE_SFMLOUTPUT_HPP
