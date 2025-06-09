//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_PERC_CRITIC_H
#define RL_GAME_PERC_CRITIC_H
#pragma once
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>

class Perc_Critic {
public:
    std::vector<double> weights;
    std::vector<double> input_action;
    double bias = 0.0;
    double given_reard = 0.0;
    double predicted_reward = 0.0;

    void set_Reward(double reward);
    double averageValidWeight() const;
    void learn(double learning_rate);
    double calculate();
    void ensureInitialized(size_t expected_size);
    void saveToStream(std::ofstream& out) const;
    void loadFromStream(std::ifstream& in);
};

#endif //RL_GAME_PERC_CRITIC_H
