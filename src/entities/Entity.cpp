//
// Created by david on 11/4/2025.
//

#include "entities/Entity.hpp"
#include "graphics/SfmlOutput.hpp"

Entity::Entity() : m_velocity(0.f, 0.f), m_movement(0.f, 0.f), m_isMoving(false), m_onGround(false), m_isAttacking(false), m_currentFacingDirection(false) {
    // Set the clocks on stop and time=0 by default
    m_activeAttackClock.reset();
    m_cooldownAttackClock.reset();
}

sf::Vector2f Entity::getPosition() const {
    return m_shape.getPosition();
}

bool Entity::getAttackingState() const {
    return m_isAttacking;
}

sf::FloatRect Entity::getBounds() const {
    return m_shape.getGlobalBounds();
}

void Entity::setPosition(const sf::Vector2f& position) {
    m_shape.setPosition(position);
}

TerrainMove Entity::resolveTerrainMovement(const sf::FloatRect& body, const TerrainCollision& terrain, const sf::Vector2f& displacement) {
    const TerrainMove moved = terrain.resolveMovement(body, displacement);
    setPosition(moved.bounds.position);
    return moved;
}

// Non-virtual interface: the stream operator forwards to the virtual print(), so an entity held through an Entity pointer prints as its real type.
std::ostream& operator<<(std::ostream& os, const Entity& e) {
    e.print(os);
    return os;
}

void Entity::print(std::ostream& os) const {
    os << "position " << getPosition() << ", velocity " << m_velocity << ", facing " << (m_currentFacingDirection ? "right" : "left") << ", on ground " << (m_onGround ? "yes" : "no")
       << ", attacking " << (m_isAttacking ? "yes" : "no");
}
