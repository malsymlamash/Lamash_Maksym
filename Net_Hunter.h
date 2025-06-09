//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_NET_HUNTER_H
#define RL_GAME_NET_HUNTER_H
#pragma once
#include <vector>
#include <map>
#include <string>
#include <utility>
#include "Perc_Sector.h"
#include "Perc_XY_coo.h"
#include "Perc_Vector.h"

class Net_Hunter {
public:
    std::vector<Perc_Sector> layer_0;
    std::vector<Perc_XY_coo> layer_1_x;
    std::vector<Perc_XY_coo> layer_1_y;
    Perc_Vector layer_2;

    std::map<int, std::vector<std::pair<double, double>>>* sector_points_ptr = nullptr;

    explicit Net_Hunter(int sectors);

    void setSectorPoints(std::map<int, std::vector<std::pair<double, double>>>* ptr);
    void setCriticLearning(bool enabled);

    std::vector<double> process(const std::map<std::string, double>& input_map);
    int getChosenSector(const std::map<std::string, double>& input_map);

    void saveToFile(const std::string& path) const;
    void loadFromFile(const std::string& path);
    void loadFromFileWithoutCritic(const std::string& path);
};

#endif //RL_GAME_NET_HUNTER_H
