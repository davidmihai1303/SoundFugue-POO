//
// Created by david on 10/31/2025.
//

#ifndef SOUNDFUGUE_PLAYER_HPP
#define SOUNDFUGUE_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "entities/Entity.hpp"
#include "game/InputState.hpp"
#include "game/Constants.hpp"
#include "graphics/Animation.hpp"

class Player final : public Entity {
public:

    explicit Player(const sf::Texture& standingTexture, const sf::Texture& walkingTexture, const sf::Texture& attackingTexture);

    void update(sf::Time dt, const TerrainCollision& terrain) override;

    void draw(sf::RenderTarget& target) const override;

    sf::Vector2f movementLogic(sf::Time dt, bool hasGroundSupport) override;

    void attackingLogic() override;

    void setInputState(const InputState& inputState);

    void setPosition(const sf::Vector2f& position) override;

    // Death reset: ends the attack, clears motion and input-derived state, restores the dash and the attack cooldown,
    // faces right, then moves the body and sprites. Ground support comes from the next update's pre-move check.
    void respawn(const sf::Vector2f& position);

    // Code=1
    sf::FloatRect getAttackingBounds() const;

private:
    // --- Sprites
    sf::Sprite m_standingSprite;
    sf::Sprite m_walkingSprite;
    sf::Sprite m_attackingSprite;
    // --------

    InputState m_inputState;
    sf::Time m_cooldownAttackTime = sf::seconds(Constants::Player::AttackCooldown);
    sf::Time m_activeAttackTime = sf::seconds(Constants::Player::ActiveAttackDuration);

    // Code=1
    sf::RectangleShape attackingShape;

    // Attacking in-air helpers
    bool m_isFrozen;

    bool m_shiftFromGround; // If you pressed Shift while you were on ground
    // We need this variable to tell whether the jump was while moving or while running, regardless of pressing Shift in-air

    bool m_dashAttack;
    bool m_hasDashed;

    // --- Animation Variables ---
    unsigned int m_animationToDraw; // What animation to draw on screen: 0 - standing, 1 - walking, 2 - attack

    // Configured once in the constructor and never played: switching to an animation copies it into m_activeAnimation
    Animation m_standingAnimation;
    Animation m_walkingAnimation;
    Animation m_attackingAnimation;

    // The animation currently playing, on the sprite chosen by m_animationToDraw
    Animation m_activeAnimation;
    // ---------------------------

    void print(std::ostream& os) const override;
    void attack();
    void resetDash();

    // Ends the current attack: stops its timer, releases an airborne freeze with zero velocity and clears the dash.
    // Leaves the cooldown and dash availability (m_hasDashed) unchanged.
    void cancelAttack();

    void animationLogic(sf::Time dt);
};

#endif //SOUNDFUGUE_PLAYER_HPP
