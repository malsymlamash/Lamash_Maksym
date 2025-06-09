//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_GUI_H
#define RL_GAME_GUI_H
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

void saveProgressToFile(const std::string& filename, int resourceCount, const int stats[3]);
bool loadProgressFromFile(const std::string& filename, int& resourceCount, int stats[3]);
bool showCharacterWindow(sf::Texture& characterTexture, const std::string& characterName, bool isPuppet, int& resourceCount);

#endif //RL_GAME_GUI_H
