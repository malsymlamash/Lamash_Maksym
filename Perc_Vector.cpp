//
// Created by 2005s on 18.05.2025.
//

#include "Perc_Vector.h"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "EnvUtils.h"

Perc_Vector::Perc_Vector() {
    weights_x.resize(expected_keys_x.size());
    for (auto& w : weights_x)
        w = ((rand() / (double)RAND_MAX) * 2 - 1);

    weights_y.resize(expected_keys_y.size());
    for (auto& w : weights_y)
        w = ((rand() / (double)RAND_MAX) * 2 - 1);
}

void Perc_Vector::setLearningMode(bool use_critic) {
    use_critic_learning = use_critic;
}

std::vector<double> Perc_Vector::calculateVec(const std::map<std::string, double>& input_map) {
    double dx = bias_x;
    double dy = bias_y;

    for (int i = 0; i < weights_x.size(); ++i) {
        const std::string& key = expected_keys_x[i];
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key == "dx" || key == "sector_center_x" || key == "runner_x" || key == "chaser_x" || key == "distance_x" || key == "nearest_bush_dx")
                val /= 800.0;
            dx += weights_x[i] * val;
        }
    }

    for (int i = 0; i < weights_y.size(); ++i) {
        const std::string& key = expected_keys_y[i];
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key == "dy" || key == "sector_center_y" || key == "runner_y" || key == "chaser_y" || key == "distance_y" || key == "nearest_bush_dy")
                val /= 600.0;
            dy += weights_y[i] * val;
        }
    }

    auto norm = EnvUtils::normalize_vector(dx, dy);

    critic_x.input_action.clear();
    critic_y.input_action.clear();

    for (const std::string& key : expected_keys_x) {
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key == "dx") val /= 800.0;
            critic_x.input_action.push_back(val);
        }
    }
    critic_x.input_action.push_back(dx);

    for (const std::string& key : expected_keys_y) {
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key == "dy") val /= 600.0;
            critic_y.input_action.push_back(val);
        }
    }
    critic_y.input_action.push_back(dy);

    return {norm[0] * 2.0, norm[1] * 2.0};
}

void Perc_Vector::giveReward(double reward) {
    past_reward = current_reward;
    current_reward = reward;

    critic_x.ensureInitialized(critic_x.input_action.size());
    critic_x.set_Reward(reward);
    critic_x.learn(0.02);

    critic_y.ensureInitialized(critic_y.input_action.size());
    critic_y.set_Reward(reward);
    critic_y.learn(0.02);
}

void Perc_Vector::mutate(double base_mutation_power) {
    if (use_critic_learning) {
        if (critic_x.weights.size() != weights_x.size())
            critic_x.weights.resize(weights_x.size(), 0.0);
        if (critic_y.weights.size() != weights_y.size())
            critic_y.weights.resize(weights_y.size(), 0.0);

        for (int i = 0; i < weights_x.size(); ++i) {
            double diff = critic_x.weights[i] - weights_x[i];
            weights_x[i] += diff * base_mutation_power;
        }

        for (int i = 0; i < weights_y.size(); ++i) {
            double diff = critic_y.weights[i] - weights_y[i];
            weights_y[i] += diff * base_mutation_power;
        }

        bias_x += (critic_x.bias - bias_x) * base_mutation_power;
        bias_y += (critic_y.bias - bias_y) * base_mutation_power;

    } else {
        double delta = current_reward - past_reward;
        double factor = (delta >= 0) ? std::max(0.1, 1.0 - delta) : 1.0 + std::min(1.0, -delta);
        double adjusted_power = base_mutation_power * factor;

        for (int i = 0; i < weights_x.size(); ++i)
            weights_x[i] += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;
        for (int i = 0; i < weights_y.size(); ++i)
            weights_y[i] += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;

        bias_x += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;
        bias_y += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;
    }
}

void Perc_Vector::saveToFile(std::ofstream& out) const {
    critic_x.saveToStream(out);
    critic_y.saveToStream(out);
    out << bias_x << " " << bias_y << "\n";

    out << weights_x.size() << "\n";
    for (double w : weights_x) out << w << " ";
    out << "\n";

    out << weights_y.size() << "\n";
    for (double w : weights_y) out << w << " ";
    out << "\n";
}

void Perc_Vector::loadFromFile(std::ifstream& in) {
    critic_x.loadFromStream(in);
    critic_y.loadFromStream(in);

    in >> bias_x >> bias_y;

    size_t size_x, size_y;
    in >> size_x;
    weights_x.resize(size_x);
    for (size_t i = 0; i < size_x; ++i) in >> weights_x[i];

    in >> size_y;
    weights_y.resize(size_y);
    for (size_t i = 0; i < size_y; ++i) in >> weights_y[i];

    critic_x.ensureInitialized(weights_x.size());
    critic_y.ensureInitialized(weights_y.size());
}

void Perc_Vector::loadFromFileWithoutCritic(std::ifstream& in) {
    if (!(in >> bias_x >> bias_y)) {
        std::cerr << "[Perc_Vector] Error: Failed to read biases\n";
        return;
    }

    size_t size_x, size_y;
    if (!(in >> size_x)) {
        std::cerr << "[Perc_Vector] Error: Failed to read size_x\n";
        return;
    }

    weights_x.resize(size_x);
    for (size_t i = 0; i < size_x; ++i) {
        if (!(in >> weights_x[i])) {
            std::cerr << "[Perc_Vector] Error reading weight_x[" << i << "]\n";
            return;
        }
    }

    if (!(in >> size_y)) {
        std::cerr << "[Perc_Vector] Error: Failed to read size_y\n";
        return;
    }

    weights_y.resize(size_y);
    for (size_t i = 0; i < size_y; ++i) {
        if (!(in >> weights_y[i])) {
            std::cerr << "[Perc_Vector] Error reading weight_y[" << i << "]\n";
            return;
        }
    }
}


