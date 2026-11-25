#include "graphics/Animation.hpp"
#include "graphics/SfmlOutput.hpp"

// A horizontal strip: every frame is as tall as the texture and frameCount frames share its width
Animation::Animation(const sf::Vector2u textureSize, const unsigned int frameCount, const float frameDuration)
    : m_frameSize(textureSize.x / frameCount, textureSize.y), m_frameCount(frameCount), m_frameDuration(frameDuration), m_currentFrame(0), m_elapsedTime(0.f) {}

// The playback position belongs to the animation that was running, not to the new one, so the copy starts from the first frame
Animation::Animation(const Animation& other)
    : m_frameSize(other.m_frameSize), m_frameCount(other.m_frameCount), m_frameDuration(other.m_frameDuration), m_currentFrame(0), m_elapsedTime(0.f) {}

Animation& Animation::operator=(const Animation& other) {
    if (this != &other) {
        m_frameSize = other.m_frameSize;
        m_frameCount = other.m_frameCount;
        m_frameDuration = other.m_frameDuration;
        // Restart instead of continuing from where either animation was
        m_currentFrame = 0;
        m_elapsedTime = 0.f;
    }
    return *this;
}

Animation::~Animation() = default;

bool Animation::advance(const sf::Time dt) {
    m_elapsedTime += dt.asSeconds();

    // Not enough time on this frame yet
    if (m_elapsedTime < m_frameDuration)
        return false;

    // Keep the time past the frame's end, so the next frame does not start late
    m_elapsedTime -= m_frameDuration;
    m_currentFrame++;
    // Loop back to the first frame (0 -> 1 -> ... -> last -> 0)
    if (m_currentFrame >= m_frameCount)
        m_currentFrame = 0;
    return true;
}

sf::IntRect Animation::currentFrameRect() const {
    // Frame n starts at n * frame width; the strip has a single row, so the top is always 0
    const int left = static_cast<int>(m_currentFrame * m_frameSize.x);
    return {{left, 0}, sf::Vector2i(m_frameSize)};
}

std::ostream& operator<<(std::ostream& os, const Animation& animation) {
    os << "Animation{frame " << animation.m_currentFrame + 1 << "/" << animation.m_frameCount << ", frame size " << animation.m_frameSize << ", " << animation.m_frameDuration << "s per frame}";
    return os;
}
