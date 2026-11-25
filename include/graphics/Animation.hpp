#ifndef SOUNDFUGUE_ANIMATION_HPP
#define SOUNDFUGUE_ANIMATION_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <ostream>

// Plays a horizontal spritesheet strip: which frame is on screen and for how long.
// The configuration (frame size, frame count, frame duration) is copied between animations, the playback position is not:
// a copy always starts from the first frame.
class Animation {
public:
    friend std::ostream& operator<<(std::ostream& os, const Animation& animation);

    Animation(sf::Vector2u textureSize, unsigned int frameCount, float frameDuration);

    // Takes over the configuration of other and starts from the first frame
    Animation(const Animation& other);

    // Takes over the configuration of other and restarts from the first frame
    Animation& operator=(const Animation& other);

    ~Animation();

    // Advances the playback by dt. Returns true when the frame on screen changed.
    bool advance(sf::Time dt);

    // The part of the spritesheet that shows the current frame
    [[nodiscard]] sf::IntRect currentFrameRect() const;

private:
    // --- Configuration: copied
    sf::Vector2u m_frameSize; // Width and height of a single frame
    unsigned int m_frameCount;
    float m_frameDuration; // How long one frame stays on screen (in seconds)

    // --- Playback position: reset on every copy
    unsigned int m_currentFrame; // Starting from 0
    float m_elapsedTime;         // Time spent on the current frame (in seconds)
};

#endif // SOUNDFUGUE_ANIMATION_HPP
