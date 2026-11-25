//
// Created by david on 10/31/2025.
//

#include "entities/Player.hpp"
#include <cmath>

Player::Player(const sf::Texture& standingTexture, const sf::Texture& walkingTexture,
               const sf::Texture& attackingTexture)
    : // --- Sprites
      m_standingSprite(standingTexture), m_walkingSprite(walkingTexture), m_attackingSprite(attackingTexture),

      m_isFrozen(false), m_shiftFromGround(false), m_dashAttack(false), m_hasDashed(false),

      // --- Animations
      m_animationToDraw(0),

      m_standingAnimation(standingTexture.getSize(), Constants::Player::Animation::StandingFrameCount, Constants::Player::Animation::StandingAnimDuration),
      m_walkingAnimation(walkingTexture.getSize(), Constants::Player::Animation::WalkingFrameCount, Constants::Player::Animation::WalkingAnimDuration),
      m_attackingAnimation(attackingTexture.getSize(), Constants::Player::Animation::AttackingFrameCount, Constants::Player::Animation::AttackingAnimDuration),
      m_activeAnimation(m_standingAnimation) {
    // Create the player's hitbox
    m_shape.setSize(sf::Vector2f({Constants::Player::HitboxWidth, Constants::Player::HitboxHeight}));
    m_shape.setFillColor(Constants::Player::HitboxColor);
    m_shape.setPosition({0.f, 250.f});
    m_currentFacingDirection = true; // Different from all the other entities (enemies)
    m_lastFacingDirection = true;

    // Every sprite starts on the first frame of its animation
    const sf::IntRect standingFrame = m_standingAnimation.currentFrameRect();
    const sf::IntRect walkingFrame = m_walkingAnimation.currentFrameRect();
    const sf::IntRect attackingFrame = m_attackingAnimation.currentFrameRect();

    // -------- STANDING ANIMATION --------
    m_standingSprite.setTextureRect(standingFrame);
    m_standingSprite.setOrigin({static_cast<float>(standingFrame.size.x) / 2 + 5.f, static_cast<float>(standingFrame.size.y)}); //origin in the middle bottom with offset to match the body

    //     --------- WALKING ANIMATION --------
    m_walkingSprite.setTextureRect(walkingFrame);
    m_walkingSprite.setOrigin({static_cast<float>(walkingFrame.size.x) / 2 + 5.f, static_cast<float>(walkingFrame.size.y)}); //origin in the middle bottom with offset to match the body

    //    --------- ATTACKING ANIMATION --------
    m_attackingSprite.setTextureRect(attackingFrame);
    m_attackingSprite.setOrigin({static_cast<float>(standingFrame.size.x) / 2 + 5.f, static_cast<float>(attackingFrame.size.y)}); // We keep the same x from the standing/walking animation
}

void Player::update(const sf::Time dt, const TerrainCollision& terrain) {
    // Ground support is deliberately queried on both sides of the movement.
    // Before: movementLogic gates the jump on it, so it has to describe where she stands right
    // now. This is also what catches a respawn teleport, which World::handleCollisions performs
    // after the previous update() has already returned.
    const sf::Vector2f displacement = movementLogic(dt, terrain.hasGroundSupport(m_shape.getGlobalBounds()));
    const TerrainMove moved = resolveTerrainMovement(m_shape.getGlobalBounds(), terrain, displacement);

    // After: attackingLogic() below has to see the ground state she ended this frame in, or
    // attacking on the exact frame she lands or steps off a ledge takes the wrong branch in
    // attack(). The shape already holds the resolved position here, because
    // resolveTerrainMovement applied it before returning.
    // This asks the terrain about her final position rather than reading moved.contacts.floor:
    // a floor contact only means she met a floor at some point during the sweep, which is not the
    // same as standing on one afterwards. See the "land then slide off ledge" case in
    // tests/TerrainCollisionTests.cpp.
    if (terrain.hasGroundSupport(m_shape.getGlobalBounds()))
        m_onGround = true;
    else
        m_onGround = false;
    // Zero out velocity between frames in case of contact.
    // Only the component pointing into the contacted surface is cleared, so motion
    // along the surface and motion away from it are preserved.
    if (moved.contacts.floor && m_velocity.y > 0.f)
        m_velocity.y = 0.f;
    if (moved.contacts.ceiling && m_velocity.y < 0.f)
        m_velocity.y = 0.f;
    if (moved.contacts.leftWall && m_velocity.x < 0.f)
        m_velocity.x = 0.f;
    if (moved.contacts.rightWall && m_velocity.x > 0.f)
        m_velocity.x = 0.f;

    attackingLogic();
    animationLogic(dt);

    m_lastFacingDirection = m_currentFacingDirection; // update for next frame
}

sf::Vector2f Player::movementLogic(const sf::Time dt, bool hasGroundSupport) {
    // Support measured before this frame's movement; see the note in Player::update.
    if (hasGroundSupport) {
        resetDash();
        m_onGround = true;
    } else
        m_onGround = false;

    // Left-Right movement
    m_movement = sf::Vector2f(0.f, 0.f);
    if (!m_isFrozen) {
        // so the player can't move while he's attacking in air
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
            m_movement.x -= Constants::Player::MoveSpeed;
            m_currentFacingDirection = false;
            m_isMoving = true;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
            m_movement.x += Constants::Player::MoveSpeed;
            m_currentFacingDirection = true;
            m_isMoving = true;
        } else {
            m_shiftFromGround = false;
            m_isMoving = false;
        }
    }

    // Jumping logic and gravity
    // No jump while frozen: a frozen attack must finish first
    if (m_onGround && !m_isFrozen && sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space)) {
        m_velocity.y = -Constants::Player::JumpStrength;
        m_onGround = false;
        // Stop attacking only when jump begins
        if (m_isAttacking) {
            cancelAttack();
        }
    }
    if (!m_isFrozen)
        m_velocity.y += Constants::Physics::Gravity * dt.asSeconds();
    // Make sure gravity doesn't stack up while frozen in-air

    // Sprint Logic
    if (m_inputState.shiftDown) {
        m_movement.x *= Constants::Player::SprintMultiplier;
        if (m_onGround)
            m_shiftFromGround = true;
    }

    if (!m_inputState.shiftDown && m_onGround)
        m_shiftFromGround = false;

    // Stop moving if attacking
    if (m_isAttacking && !m_dashAttack) {
        if (m_inputState.firstPressed != 's')
            m_movement.x = 0.f;
        else
            m_movement.x *= Constants::Player::RunningAttackBoost;
    }

    // Smoothen dash attack so it decreases in speed over time
    if (m_dashAttack) {
        // Using this so we don't need exactly 60 frames epr second
        // We want to apply 0.985 roughly 60 times per second
        // Formula: factor ^ (60 * dt)
        const float friction = std::pow(Constants::Player::DashFriction, 60.f * dt.asSeconds());
        m_velocity.x *= friction;
    }

    // Flip the sprites
    if (m_lastFacingDirection != m_currentFacingDirection) {
        m_standingSprite.setScale({-1.f * m_standingSprite.getScale().x, 1.f});
        m_walkingSprite.setScale({-1.f * m_walkingSprite.getScale().x, 1.f});
        m_attackingSprite.setScale({-1.f * m_attackingSprite.getScale().x, 1.f});
    }

    return (m_movement + m_velocity) * dt.asSeconds();
}

void Player::attackingLogic() {
    // Reset the attack cooldown
    if (m_cooldownAttackClock.getElapsedTime() >= m_cooldownAttackTime)
        m_cooldownAttackClock.reset();
    // Reset the active attack
    if (m_activeAttackClock.getElapsedTime() >= m_activeAttackTime) {
        cancelAttack();
    }

    if (m_isAttacking) {
        // Stop attacking if changing facing direction
        if (m_currentFacingDirection != m_lastFacingDirection) {
            cancelAttack();
        }
    }

    // Draw a rectangle representing the hitbox of the Hit-Area
    attackingShape.setSize(sf::Vector2f({Constants::Player::AttackingHitboxWidth, Constants::Player::AttackingHitboxHeight}));
    attackingShape.setFillColor(Constants::Player::AttackingHitboxColor);
    if (m_currentFacingDirection) {
        // facing right
        attackingShape.setOrigin({0.f, 0.f}); // Set origin back to default
        attackingShape.setPosition(m_shape.getPosition() + sf::Vector2f(Constants::Player::HitboxWidth, 0.f));
    } else {
        // facing left
        attackingShape.setOrigin({attackingShape.getLocalBounds().size.x, 0.f}); // Set origin to the top-right corner
        attackingShape.setPosition(m_shape.getPosition());
    }
    if (m_inputState.hasClicked) {
        attack();
    }
}

void Player::attack() {
    // if cooldown has passed
    if (m_cooldownAttackClock.getElapsedTime() == sf::Time::Zero && !m_dashAttack) {
        m_isAttacking = true;
        m_cooldownAttackClock.start();
        // Using restart() to be able to spam-attack
        m_activeAttackClock.restart();
        // Restart the animation, also when an attack is already playing (operator= starts it from the first frame)
        m_activeAnimation = m_attackingAnimation;
        m_attackingSprite.setTextureRect(m_activeAnimation.currentFrameRect());

        if (!m_onGround) {
            // We are already in the air when starting the attack
            m_isFrozen = true;
            if (m_shiftFromGround && !m_hasDashed) {
                m_hasDashed = true;
                // Running jump - keep moving forward, no A/D control
                m_dashAttack = true;
                constexpr float dashSpeed = Constants::Player::DashSpeed;
                m_velocity.x = m_currentFacingDirection ? dashSpeed : -dashSpeed;
                m_velocity.y = 0.f;
            } else {
                // Neutral jump -> classic freeze in air
                m_velocity = {0.f, 0.f};
            }
        }
    }
}

void Player::cancelAttack() {
    m_isAttacking = false;
    m_activeAttackClock.reset();
    if (m_isFrozen) {
        m_velocity = {0.f, 0.f};
        m_isFrozen = false;
    }
    m_dashAttack = false;
}

void Player::animationLogic(const sf::Time dt) {
    // Pick the animation of the current state: 0 - standing, 1 - walking, 2 - attack
    unsigned int animationIndex = 1;
    const Animation* stateAnimation = &m_walkingAnimation;
    sf::Sprite* stateSprite = &m_walkingSprite;
    if (m_isAttacking) {
        animationIndex = 2;
        stateAnimation = &m_attackingAnimation;
        stateSprite = &m_attackingSprite;
    } else if (!m_isMoving) {
        animationIndex = 0;
        stateAnimation = &m_standingAnimation;
        stateSprite = &m_standingSprite;
    }

    // On a state change the new animation starts from its first frame (operator= restarts it)
    if (animationIndex != m_animationToDraw) {
        m_animationToDraw = animationIndex; // Tell which sprite to draw
        m_activeAnimation = *stateAnimation;
        (*stateSprite).setTextureRect(m_activeAnimation.currentFrameRect());
    }

    // Sprinting also makes the walking animation play faster
    sf::Time animationTime = dt;
    if (animationIndex == 1 && m_inputState.shiftDown)
        animationTime = dt * Constants::Player::SprintMultiplier;

    if (m_activeAnimation.advance(animationTime))
        (*stateSprite).setTextureRect(m_activeAnimation.currentFrameRect());

    // --- SYNC POSITION ---
    // Snap sprite to hitbox
    (*stateSprite).setPosition({
        m_shape.getPosition().x + Constants::Player::HitboxWidth / 2,
        m_shape.getPosition().y + Constants::Player::HitboxHeight // bottom center of the hitbox
    });
}

void Player::setPosition(const sf::Vector2f& position) {
    m_shape.setPosition(position);
    m_standingSprite.setPosition({m_shape.getPosition().x + Constants::Player::HitboxWidth / 2, m_shape.getPosition().y + Constants::Player::HitboxHeight});
    m_walkingSprite.setPosition({m_shape.getPosition().x + Constants::Player::HitboxWidth / 2, m_shape.getPosition().y + Constants::Player::HitboxHeight});
    m_attackingSprite.setPosition({m_shape.getPosition().x + Constants::Player::HitboxWidth / 2, m_shape.getPosition().y + Constants::Player::HitboxHeight});
}

void Player::respawn(const sf::Vector2f& position) {
    cancelAttack();
    m_velocity = {0.f, 0.f};
    m_movement = {0.f, 0.f};
    m_isMoving = false;
    m_shiftFromGround = false;
    resetDash();
    m_cooldownAttackClock.reset();

    // Face right, as at spawn; the sprites always match the current facing here, so mirror them back if needed
    if (!m_currentFacingDirection) {
        m_standingSprite.setScale({-1.f * m_standingSprite.getScale().x, 1.f});
        m_walkingSprite.setScale({-1.f * m_walkingSprite.getScale().x, 1.f});
        m_attackingSprite.setScale({-1.f * m_attackingSprite.getScale().x, 1.f});
    }
    m_currentFacingDirection = true;
    m_lastFacingDirection = true;

    // Death happens after animationLogic() has already picked this frame's sprite, so pick the standing pose here
    m_animationToDraw = 0;
    m_activeAnimation = m_standingAnimation;
    m_standingSprite.setTextureRect(m_activeAnimation.currentFrameRect());

    setPosition(position);
}

void Player::draw(sf::RenderTarget& target) const {
    target.draw(m_shape);

    if (m_isAttacking) {
        target.draw(attackingShape);
    }

    switch (m_animationToDraw) {
    case 0:
        target.draw(m_standingSprite);
        break;

    case 1:
        target.draw(m_walkingSprite);
        break;

    case 2:
        target.draw(m_attackingSprite);
        break;

    default:
        throw std::runtime_error("Invalid animation index");
    }
}

// ----- Auxiliar funcs

void Player::setInputState(const InputState& inputState) {
    m_inputState = inputState;
}

// Code=1
sf::FloatRect Player::getAttackingBounds() const {
    return attackingShape.getGlobalBounds();
}

void Player::resetDash() {
    m_hasDashed = false;
}

// -------

void Player::print(std::ostream& os) const {
    const char* animationName = m_animationToDraw == 0 ? "standing" : (m_animationToDraw == 1 ? "walking" : "attacking");
    os << "Player{";
    Entity::print(os);
    os << ", frozen " << (m_isFrozen ? "yes" : "no") << ", dash attack " << (m_dashAttack ? "yes" : "no") << ", dash used " << (m_hasDashed ? "yes" : "no") << ", animation " << animationName << " " << m_activeAnimation << "}";
}
