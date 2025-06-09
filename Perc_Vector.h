//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_PERC_VECTOR_H
#define RL_GAME_PERC_VECTOR_H
#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>

#include "IAgent.h"
#include "EnvUtils.h"
#include "Perc_Critic.h"

#include <vector>
#include <string>
#include <map>
#include "IAgent.h"
#include "Perc_Critic.h"

class Perc_Vector : public IAgent {
public:
    std::vector<double> weights_x;
    std::vector<double> weights_y;
    double bias_x = 0.0;
    double bias_y = 0.0;
    double past_reward = 0.0;
    double current_reward = 0.0;
    Perc_Critic critic_x;
    Perc_Critic critic_y;
    bool use_critic_learning = false;

    std::vector<std::string> expected_keys_x = {
            "dx", "runner_x", "chaser_x", "distance_x", "sector_center_x",
            "nearest_bush_dx", "in_bush", "enemy_in_bush", "visible_enemy"
    };

    std::vector<std::string> expected_keys_y = {
            "dy", "runner_y", "chaser_y", "distance_y", "sector_center_y",
            "nearest_bush_dy", "in_bush", "enemy_in_bush", "visible_enemy"
    };


    Perc_Vector();

    void setLearningMode(bool use_critic);
    std::vector<double> calculateVec(const std::map<std::string, double>& input_map);
    void giveReward(double reward) override;
    void mutate(double base_mutation_power) override;

    void saveToFile(std::ofstream& out) const;
    void loadFromFile(std::ifstream& in);
    void loadFromFileWithoutCritic(std::ifstream& in);
};


#endif //RL_GAME_PERC_VECTOR_H
