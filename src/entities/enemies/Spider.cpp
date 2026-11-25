//
// Created by david on 9/22/2026.
//

#include "entities/enemies/Spider.hpp"
#include "game/Constants.hpp"

Spider::Spider(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Texture& walkingTexture, const Animation& walkingAnimation)
    : Enemy(position, size), m_walkingSprite(walkingTexture), m_walkingAnimation(walkingAnimation) {
    m_shape.setFillColor(sf::Color::Red);

    const sf::IntRect walkingFrame = m_walkingAnimation.currentFrameRect();
    m_walkingSprite.setTextureRect(walkingFrame);
    m_walkingSprite.setOrigin({static_cast<float>(walkingFrame.size.x) / 2, static_cast<float>(walkingFrame.size.y)}); //origin in the middle bottom
    m_walkingSprite.setScale({1.3f, 1.3f});
    if (!m_currentFacingDirection)
        m_walkingSprite.setScale({-1.f * m_walkingSprite.getScale().x, m_walkingSprite.getScale().y});
}

sf::Vector2f Spider::movementLogic(const sf::Time dt, bool hasGroundSupport) {
    m_movement = {0.f, 0.f};
    // As long as there is no horizontal collision, the spider just moves , otherwise it changes direction
    if (m_currentFacingDirection) {
        m_movement.x += Constants::Spider::Speed;
    } else
        m_movement.x -= Constants::Spider::Speed;

    return (m_velocity + m_movement) * dt.asSeconds();
}

void Spider::animationLogic(const sf::Time dt) {
    if (m_walkingAnimation.advance(dt))
        m_walkingSprite.setTextureRect(m_walkingAnimation.currentFrameRect());

    // --- SYNC POSITION ---
    // Snap sprite to hitbox
    m_walkingSprite.setPosition({
        m_shape.getPosition().x + Constants::Spider::HitboxWidth / 2,
        m_shape.getPosition().y + Constants::Spider::HitboxHeight // bottom center of the hitbox
    });
}

void Spider::contactLogic(const TerrainContacts& contacts) {
    if (contacts.leftWall || contacts.rightWall) {
        m_currentFacingDirection = !m_currentFacingDirection;
        // Flip the sprite
        m_walkingSprite.setScale({-1.f * m_walkingSprite.getScale().x, m_walkingSprite.getScale().y});
    }
}

void Spider::draw(sf::RenderTarget& target) const {
    target.draw(m_shape);
    target.draw(m_walkingSprite);
}

void Spider::print(std::ostream& os) const {
    os << "Spider{";
    Enemy::print(os);
    os << ", walking " << m_walkingAnimation << "}";
}
