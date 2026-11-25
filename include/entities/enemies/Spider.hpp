//
// Created by david on 9/22/2026.
//

#ifndef SOUNDFUGUE_SPIDER_HPP
#define SOUNDFUGUE_SPIDER_HPP

#include <SFML/Graphics.hpp>
#include "entities/Enemy.hpp"
#include "graphics/Animation.hpp"

class Spider final : public Enemy {
public:
    // walkingAnimation is copied: every spider starts its walk from the first frame
    Spider(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Texture& walkingTexture, const Animation& walkingAnimation);

    void draw(sf::RenderTarget& target) const override;

    sf::Vector2f movementLogic(sf::Time dt, bool hasGroundSupport) override;

protected:
    void animationLogic(sf::Time dt) override;
    void contactLogic(const TerrainContacts& contacts) override;
    void print(std::ostream& os) const override;

private:
    sf::Sprite m_walkingSprite;
    Animation m_walkingAnimation;
};

#endif //SOUNDFUGUE_SPIDER_HPP
