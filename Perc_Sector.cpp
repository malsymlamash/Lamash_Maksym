//
// Created by 2005s on 18.05.2025.
//

#include "Perc_Sector.h"
#include <cmath>
#include <iostream>
#include <cstdlib>

Perc_Sector::Perc_Sector(int sector_id_, bool isHunter_)
        : sector_id(sector_id_), isHunter(isHunter_) {
    weights.resize(expected_keys.size());
    for (auto& w : weights)
        w = ((rand() / (double)RAND_MAX) * 2 - 1);
}

void Perc_Sector::setLearningMode(bool use_critic) {
    use_critic_learning = use_critic;
}

int Perc_Sector::getSectorId() const {
    return sector_id;
}

double Perc_Sector::calculate(const std::map<std::string, double>& input_map, int sector_being_evaluated) {
    double sum = bias;
    for (size_t i = 0; i < weights.size(); ++i) {
        const std::string& key = expected_keys[i];
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key.find("_x") != std::string::npos) val /= 800.0;
            if (key.find("_y") != std::string::npos) val /= 600.0;
            sum += weights[i] * val;
        }
    }

    double dx = input_map.at("runner_x") - input_map.at("chaser_x");
    double dy = input_map.at("runner_y") - input_map.at("chaser_y");
    double dist = std::sqrt(dx * dx + dy * dy);

    if (isHunter) {
        bool victim_right = input_map.at("runner_x") > 400;
        bool victim_bottom = input_map.at("runner_y") > 300;
        int victim_sector = victim_bottom * 2 + victim_right;

        if (sector_being_evaluated == victim_sector)
            sum += 5.0;

        sum += (1.0 / (dist + 1e-6)) * attraction_weight;
    } else {
        bool chaser_right = input_map.at("chaser_x") > 400;
        bool chaser_bottom = input_map.at("chaser_y") > 300;
        int real_chaser_sector = chaser_bottom * 2 + chaser_right;

        if (sector_being_evaluated == real_chaser_sector)
            sum -= 3.0;

        double cx = input_map.at("sector_center_x");
        double cy = input_map.at("sector_center_y");
        double to_chaser = std::sqrt((cx - input_map.at("chaser_x")) * (cx - input_map.at("chaser_x")) +
                                     (cy - input_map.at("chaser_y")) * (cy - input_map.at("chaser_y")));

        sum += to_chaser * 0.5;
        sum -= dist * 0.3;
    }

    critic.input_action.clear();
    for (const std::string& key : expected_keys) {
        if (input_map.count(key)) {
            double val = input_map.at(key);
            if (key.find("_x") != std::string::npos) val /= 800.0;
            if (key.find("_y") != std::string::npos) val /= 600.0;
            critic.input_action.push_back(val);
        }
    }
    for (int i = 0; i < 4; ++i) {
        critic.input_action.push_back(i == sector_being_evaluated ? 1.0 : 0.0);
    }

    double noise = ((rand() % 100) / 1000.0) - 0.05;
    sum += noise;

    return sum;
}

void Perc_Sector::giveReward(double reward) {
    past_reward = current_reward;
    current_reward = reward;
    critic.ensureInitialized(critic.input_action.size());
    critic.set_Reward(reward);
    critic.learn(0.02);
}

void Perc_Sector::mutate(double base_mutation_power) {
    if (use_critic_learning) {
        if (critic.weights.size() != weights.size())
            critic.weights.resize(weights.size(), 0.0);
        for (size_t i = 0; i < weights.size(); ++i) {
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

void Perc_Sector::saveToFile(std::ofstream& out) const {
    critic.saveToStream(out);
    out << bias << "\n";
    out << weights.size() << "\n";
    for (double w : weights) out << w << " ";
    out << "\n";
}

void Perc_Sector::loadFromFile(std::ifstream& in) {
    critic.loadFromStream(in);
    in >> bias;
    size_t size;
    in >> size;
    if (!in.good() || size == 0 || size > 1000) {
        weights.assign(6, 0.0);
        bias = 0.0;
        return;
    }
    weights.resize(size);
    for (double& w : weights) in >> w;
}

void Perc_Sector::loadFromFileWithoutCritic(std::ifstream& in) {
    if (!(in >> bias)) return;
    for (double& w : weights) {
        if (!(in >> w)) return;
    }
}

