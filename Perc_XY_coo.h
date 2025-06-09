//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_PERC_XY_COO_H
#define RL_GAME_PERC_XY_COO_H
#pragma once
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "IAgent.h"
#include "Perc_Critic.h"

class Perc_XY_coo : public IAgent {
public:
    std::vector<double> weights;
    double bias = 0.0;
    double past_reward = 0.0;
    double current_reward = 0.0;
    int mode = 0;
    std::vector<std::string> expected_keys;
    Perc_Critic critic;
    bool use_critic_learning = false;

    static const std::vector<std::string> expected_keys_x;
    static const std::vector<std::string> expected_keys_y;

    Perc_XY_coo(char mode_);

    void setLearningMode(bool use_critic);
    double normalize(double value, double max_abs_val);
    double normalizeByKey(const std::string& key, double value);

    double chooseBestCoordinate(
            const std::map<std::string, double>& input_map,
            const std::vector<std::pair<double, double>>& sector_points,
            int chosen_sector_id);

    void giveReward(double reward) override;
    void mutate(double base_mutation_power) override;

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);
    void loadFromFileWithoutCritic(std::ifstream& in);
};

#endif //RL_GAME_PERC_XY_COO_H
