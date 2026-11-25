//
// Created by david on 10/31/2025.
//

#ifndef SOUNDFUGUE_GAME_HPP
#define SOUNDFUGUE_GAME_HPP
#include <SFML/Graphics.hpp>
#include <ostream>
#include "game/World.hpp"
#include "game/InputState.hpp"

class Game {
public:
    friend std::ostream& operator<<(std::ostream& os, const Game& g);

    // debugOutput receives the game state when F1 is pressed; the application decides where that goes
    explicit Game(std::ostream& debugOutput);

    void run();

private:
    void processEvents();

    void updateView(float x, float y);

    void updateCamera(sf::Time dt);

    std::ostream& m_debugOutput;
    sf::RenderWindow m_window;
    sf::View m_view;
    World m_world;
    InputState m_inputState;
};

#endif //SOUNDFUGUE_GAME_HPP
