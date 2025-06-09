//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_PERC_SECTOR_H
#define RL_GAME_PERC_SECTOR_H
#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "IAgent.h"
#include "Perc_Critic.h"

class Perc_Sector : public IAgent {
public:
    std::vector<double> weights;
    double bias = 0.0;
    double past_reward = 0.0;
    double current_reward = 0.0;
    int sector_id;
    bool isHunter;
    Perc_Critic critic;
    bool use_critic_learning = false;

    std::vector<std::string> expected_keys = {
            "runner_x", "runner_y",
            "chaser_x", "chaser_y",
            "distance_x", "distance_y",
            "in_bush",
            "enemy_in_bush",
            "visible_enemy",
            "nearest_bush_dx",
            "nearest_bush_dy",
            "bush_distance"
    };

    double attraction_weight = 10.0;

    Perc_Sector(int sector_id_, bool isHunter_ = true);

    void setLearningMode(bool use_critic);

    double calculate(const std::map<std::string, double>& input_map, int sector_being_evaluated);

    void giveReward(double reward) override;
    void mutate(double base_mutation_power) override;

    int getSectorId() const;

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);
    void loadFromFileWithoutCritic(std::ifstream& in);
};

#endif //RL_GAME_PERC_SECTOR_H
