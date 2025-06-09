//
// Created by 2005s on 18.05.2025.
//

#include "Perc_XY_coo.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

const std::vector<std::string> Perc_XY_coo::expected_keys_x = {
        "chaser_x", "sector_center_x", "distance_x", "sector_target_x", "chosen_sector_id","nearest_bush_dx"
};

const std::vector<std::string> Perc_XY_coo::expected_keys_y = {
        "chaser_y", "sector_center_y", "distance_y", "sector_target_y", "chosen_sector_id","nearest_bush_dy"
};

Perc_XY_coo::Perc_XY_coo(char mode_) {
    if (mode_ == 'x') {
        mode = 0;
        expected_keys = expected_keys_x;
    } else if (mode_ == 'y') {
        mode = 1;
        expected_keys = expected_keys_y;
    }

    weights.resize(expected_keys.size());
    for (auto& w : weights)
        w = ((rand() / (double)RAND_MAX) * 2 - 1);
}

double Perc_XY_coo::normalizeByKey(const std::string& key, double value) {
    if (key.find("_x") != std::string::npos || key == "dx" || key == "sector_target_x") return value / 800.0;
    if (key.find("_y") != std::string::npos || key == "dy" || key == "sector_target_y") return value / 600.0;
    return value;
}

double Perc_XY_coo::chooseBestCoordinate(
        const std::map<std::string, double>& input_map,
        const std::vector<std::pair<double, double>>& sector_points,
        int chosen_sector_id)
{
    double best_score = -1e9;
    double best_coord = 0.0;

    std::vector<std::string> bush_keys = { "nearest_bush_dx", "nearest_bush_dy" };
    for (const std::string& key : bush_keys) {
        if (input_map.count(key) && std::find(expected_keys.begin(), expected_keys.end(), key) == expected_keys.end()) {
            expected_keys.push_back(key);
            weights.push_back(((rand() / (double)RAND_MAX) * 2 - 1));
        }
    }

    for (const auto& [x, y] : sector_points) {
        std::map<std::string, double> tmp_input = input_map;
        tmp_input["chosen_sector_id"] = static_cast<double>(chosen_sector_id);

        if (mode == 0) tmp_input["sector_target_x"] = x;
        else tmp_input["sector_target_y"] = y;

        double sum = bias;
        double noise = ((rand() % 100) / 1000.0) - 0.05;
        sum += noise;

        for (int i = 0; i < weights.size(); ++i) {
            const std::string& key = expected_keys[i];
            if (tmp_input.count(key)) {
                double val = normalizeByKey(key, tmp_input.at(key));
                sum += weights[i] * val;
            }
        }

        if (sum > best_score) {
            best_score = sum;
            best_coord = (mode == 0) ? x : y;
        }
    }

    critic.input_action.clear();
    for (const std::string& key : expected_keys) {
        if (input_map.count(key)) {
            double val = normalizeByKey(key, input_map.at(key));
            critic.input_action.push_back(val);
        }
    }
    critic.input_action.push_back(best_coord);

    return best_coord;
}

void Perc_XY_coo::giveReward(double reward) {
    past_reward = current_reward;
    current_reward = reward;
    critic.ensureInitialized(critic.input_action.size());
    critic.set_Reward(reward);
    critic.learn(0.02);
}

void Perc_XY_coo::mutate(double base_mutation_power) {
    if (use_critic_learning) {
        if (critic.weights.size() != weights.size())
            critic.weights.resize(weights.size(), 0.0);

        for (int i = 0; i < weights.size(); ++i) {
            double diff = critic.weights[i] - weights[i];
            weights[i] += diff * base_mutation_power;
        }
        double bias_diff = critic.bias - bias;
        bias += bias_diff * base_mutation_power;
    } else {
        double delta = current_reward - past_reward;
        double factor = (delta >= 0) ? std::max(0.1, 1.0 - delta) : 1.0 + std::min(1.0, -delta);
        double adjusted_power = base_mutation_power * factor;

        for (double& w : weights)
            w += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;
        bias += ((rand() / (double)RAND_MAX) * 2 - 1) * adjusted_power;
    }
}

void Perc_XY_coo::setLearningMode(bool use_critic) {
    use_critic_learning = use_critic;
}


void Perc_XY_coo::saveToFile(std::ofstream& out) const {
    critic.saveToStream(out);
    out << bias << "\n";
    out << weights.size() << "\n";
    for (double w : weights) out << w << " ";
    out << "\n";
}

void Perc_XY_coo::loadFromFile(std::ifstream& in) {
    critic.loadFromStream(in);
    in >> bias;
    size_t size;
    in >> size;
    if (!in.good() || size == 0 || size > 1000) {
        weights.assign(expected_keys.size(), 0.0);
        bias = 0.0;
        return;
    }
    weights.resize(size);
    for (double& w : weights) in >> w;
}

void Perc_XY_coo::loadFromFileWithoutCritic(std::ifstream& in) {
    if (!(in >> bias)) {
        std::cerr << "[Perc_XY] Failed to read bias\n";
        return;
    }
    for (double& w : weights) {
        if (!(in >> w)) {
            std::cerr << "[Perc_XY] Failed to read weight\n";
            return;
        }
    }
}

