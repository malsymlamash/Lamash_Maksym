//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_ENVIRONMENT_GUI_H
#define RL_GAME_ENVIRONMENT_GUI_H

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include "EnvUtils.h"
#include "EnvironmentState.h"

class EnvironmentGUI {
public:
    explicit EnvironmentGUI(int width = 800, int height = 600);

    bool open();
    void update(const EnvironmentState& state);
    void setSectorPoints(std::map<int, std::vector<std::pair<double, double>>>* ptr);
    void toggleSectors();
    std::vector<sf::Vector2f> getBushPositions();
    sf::Sprite chaserSprite;
    sf::Sprite runnerSprite;
private:
    sf::RenderWindow window;
    sf::Font font;

    sf::Texture chaserTexture, runnerTexture;

    sf::Texture backgroundTexture, bushTexture;
    sf::Sprite backgroundSprite;
    std::vector<sf::Sprite> bushes;
    std::vector<sf::Vector2f> bushPositions;
    sf::Clock bushShakeClock;

    sf::Texture tileset;
    sf::Sprite grassTile;

    std::map<int, std::vector<std::pair<double, double>>>* sector_points_ptr = nullptr;

    bool showSectors = false;
    bool isOpen = true;

    int runnerFrame = 0;
    float runnerAnimTimer = 0.f;
    float runnerAnimSpeed = 0.25f;
    int runnerDirection = 2;

    int chaserFrame = 0;
    float chaserAnimTimer = 0.f;
    float chaserAnimSpeed = 0.25f;
    int chaserAnimRow = 0;
    bool useAltRunRow = false;

    void drawGrassBackground();
    void updateBushesShake(double runner_x, double runner_y, double chaser_x, double chaser_y);

};

#endif //RL_GAME_ENVIRONMENT_GUI_H
