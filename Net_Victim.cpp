//
// Created by 2005s on 18.05.2025.
//

// Net_Victim.cpp
#include "Net_Victim.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "EnvUtils.h"

Net_Victim::Net_Victim(int sectors) {
    for (int i = 0; i < sectors; ++i) {
        layer_0.emplace_back(i, false);
        layer_1_x.emplace_back('x');
        layer_1_y.emplace_back('y');
    }
}

void Net_Victim::setSectorPoints(std::map<int, std::vector<std::pair<double, double>>>* ptr) {
    sector_points_ptr = ptr;
}

void Net_Victim::setCriticLearning(bool enabled) {
    for (auto& n : layer_0) n.setLearningMode(enabled);
    for (auto& n : layer_1_x) n.setLearningMode(enabled);
    for (auto& n : layer_1_y) n.setLearningMode(enabled);
    layer_2.setLearningMode(enabled);
}

std::vector<double> Net_Victim::process(const std::map<std::string, double>& input_map) {
    const auto& sector_points = (*sector_points_ptr);
    double best_score = -1e9;
    int best_sector_id = 0;

    for (int i = 0; i < layer_0.size(); ++i) {
        double score = layer_0[i].calculate(input_map, i);
        if (score > best_score) {
            best_score = score;
            best_sector_id = i;
        }
    }

    std::map<std::string, double> input_augmented = input_map;
    input_augmented["chosen_sector_id"] = static_cast<double>(best_sector_id);

    const auto& points = sector_points.at(best_sector_id);
    double target_x = layer_1_x[best_sector_id].chooseBestCoordinate(input_augmented, points, best_sector_id);
    double target_y = layer_1_y[best_sector_id].chooseBestCoordinate(input_augmented, points, best_sector_id);

    double dist = EnvUtils::distance(target_x, target_y, input_map.at("chaser_x"), input_map.at("chaser_y"));
    double layer1_reward = std::clamp(dist / 800.0, 0.0, 1.0);

    layer_1_x[best_sector_id].giveReward(layer1_reward);
    layer_1_y[best_sector_id].giveReward(layer1_reward);

    std::map<std::string, double> vec_input = {
            {"dx", target_x - input_map.at("chaser_x")},
            {"dy", target_y - input_map.at("chaser_y")},
            {"runner_x", input_map.at("runner_x")},
            {"runner_y", input_map.at("runner_y")},
            {"chaser_x", input_map.at("chaser_x")},
            {"chaser_y", input_map.at("chaser_y")},
            {"distance_x", input_map.at("runner_x") - input_map.at("chaser_x")},
            {"distance_y", input_map.at("runner_y") - input_map.at("chaser_y")},
            {"sector_center_x", input_map.at("sector_center_x")},
            {"sector_center_y", input_map.at("sector_center_y")}
    };

    if (input_map.count("nearest_bush_dx")) vec_input["nearest_bush_dx"] = input_map.at("nearest_bush_dx");
    if (input_map.count("nearest_bush_dy")) vec_input["nearest_bush_dy"] = input_map.at("nearest_bush_dy");
    if (input_map.count("in_bush"))          vec_input["in_bush"] = input_map.at("in_bush");
    if (input_map.count("enemy_in_bush"))    vec_input["enemy_in_bush"] = input_map.at("enemy_in_bush");
    if (input_map.count("visible_enemy"))    vec_input["visible_enemy"] = input_map.at("visible_enemy");


    return layer_2.calculateVec(vec_input);
}

int Net_Victim::getChosenSector(const std::map<std::string, double>& input_map) {
    double best_score = -1e9;
    int best_sector_id = 0;

    for (int i = 0; i < layer_0.size(); ++i) {
        double score = layer_0[i].calculate(input_map, i);
        if (score > best_score) {
            best_score = score;
            best_sector_id = i;
        }
    }
    return best_sector_id;
}

void Net_Victim::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    out << layer_0.size() << "\n";
    for (const auto& n : layer_0) n.saveToFile(out);
    for (const auto& n : layer_1_x) n.saveToFile(out);
    for (const auto& n : layer_1_y) n.saveToFile(out);
    layer_2.saveToFile(out);
}

void Net_Victim::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    size_t count = 0;
    in >> count;

    if (!in.good() || count == 0 || count > 1000) {
        std::cerr << "[Net_Victim] Invalid layer count: " << count << "\n";
        return;
    }

    layer_0.clear();
    layer_1_x.clear();
    layer_1_y.clear();

    for (size_t i = 0; i < count; ++i) {
        layer_0.emplace_back(i, false);
        layer_1_x.emplace_back('x');
        layer_1_y.emplace_back('y');
    }

    for (size_t i = 0; i < count; ++i) layer_0[i].loadFromFile(in);
    for (size_t i = 0; i < count; ++i) layer_1_x[i].loadFromFile(in);
    for (size_t i = 0; i < count; ++i) layer_1_y[i].loadFromFile(in);
    layer_2.loadFromFile(in);
}

void Net_Victim::loadFromFileWithoutCritic(const std::string& path) {
    std::ifstream in(path);
    for (auto& neuron : layer_0) neuron.loadFromFileWithoutCritic(in);
    for (auto& neuron : layer_1_x) neuron.loadFromFileWithoutCritic(in);
    for (auto& neuron : layer_1_y) neuron.loadFromFileWithoutCritic(in);
    layer_2.loadFromFileWithoutCritic(in);
}

