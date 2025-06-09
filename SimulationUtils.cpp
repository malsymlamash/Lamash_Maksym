//
// Created by 2005s on 18.05.2025.
//

#include "SimulationUtils.h"
#include <cmath>
#include <iostream>
#include <algorithm>



HunterDifficulty hunterLevel = HunterDifficulty::EASY;
VictimDifficulty victimLevel = VictimDifficulty::EASY;

std::vector<sf::Vector2f> bushPositions = {
        {100, 100}, {200, 400}, {600, 150}, {300, 300}, {700, 500}, {400, 100}
};

// Вспомогательная функция
sf::Vector2f findClosestBush(float x, float y) {
    float minDist = 99999.f;
    sf::Vector2f closest;
    for (auto& bush : bushPositions) {
        float d = EnvUtils::distance(x, y, bush.x, bush.y);
        if (d < minDist) {
            minDist = d;
            closest = bush;
        }
    }
    return closest;
}
std::vector<double> scriptedHunterMove(const EnvironmentState& state) {
    double dx = 0.0, dy = 0.0;

    switch (hunterLevel) {
        case HunterDifficulty::EASY:
            dx = state.runner_x - state.chaser_x;
            dy = state.runner_y - state.chaser_y;
            break;

        case HunterDifficulty::MEDIUM: {
            sf::Vector2f bush = findClosestBush(state.runner_x, state.runner_y);
            dx = bush.x - state.chaser_x;
            dy = bush.y - state.chaser_y;
            break;
        }

        case HunterDifficulty::HARD: {
            static size_t bushIndex = 0;
            sf::Vector2f target = bushPositions[bushIndex];
            if (EnvUtils::distance(state.chaser_x, state.chaser_y, target.x, target.y) < 25.0) {
                bushIndex = (bushIndex + 1) % bushPositions.size();
            }
            dx = (target.x - state.chaser_x) + 0.2 * (state.runner_x - state.chaser_x);
            dy = (target.y - state.chaser_y) + 0.2 * (state.runner_y - state.chaser_y);
            break;
        }
    }

    auto norm = EnvUtils::normalize_vector(dx, dy);
    return { norm[0] * 2.0, norm[1] * 2.0 };
}
std::vector<double> scriptedVictimMove(const EnvironmentState& state) {
    double dx = 0.0, dy = 0.0;

    switch (victimLevel) {
        case VictimDifficulty::EASY:
            dx = state.runner_x - state.chaser_x;
            dy = state.runner_y - state.chaser_y;
            dx *= -1; dy *= -1;
            break;

        case VictimDifficulty::MEDIUM: {
            sf::Vector2f bush = findClosestBush(state.runner_x, state.runner_y);
            dx = bush.x - state.runner_x;
            dy = bush.y - state.runner_y;
            break;
        }

        case VictimDifficulty::HARD: {
            static size_t bushIndex = 0;
            sf::Vector2f target = bushPositions[bushIndex];
            if (EnvUtils::distance(state.runner_x, state.runner_y, target.x, target.y) < 25.0) {
                bushIndex = (bushIndex + 1) % bushPositions.size();
            }
            dx = target.x - state.runner_x;
            dy = target.y - state.runner_y;
            break;
        }
    }

    auto norm = EnvUtils::normalize_vector(dx, dy);
    return { norm[0] * 2.0, norm[1] * 2.0 };
}

void setHunterToFullyScripted(Net_Hunter& hunter_net) {
    for (int i = 0; i < hunter_net.layer_0.size(); ++i) {
        auto& neuron = hunter_net.layer_0[i];
        std::fill(neuron.weights.begin(), neuron.weights.end(), 0.0);
        neuron.bias = (i == 0) ? -1.5 : 0.0;
        neuron.critic.weights.assign(neuron.weights.size(), 0.0);
        neuron.critic.bias = 0.0;
    }

    for (int i = 0; i < hunter_net.layer_1_x.size(); ++i) {
        auto& nx = hunter_net.layer_1_x[i];
        auto& ny = hunter_net.layer_1_y[i];

        std::fill(nx.weights.begin(), nx.weights.end(), 0.0);
        std::fill(ny.weights.begin(), ny.weights.end(), 0.0);
        nx.bias = ny.bias = 0.0;

        nx.critic.weights.assign(nx.weights.size(), 0.0);
        ny.critic.weights.assign(ny.weights.size(), 0.0);

        for (int j = 0; j < nx.expected_keys.size(); ++j)
            if (nx.expected_keys[j] == "sector_target_x") nx.weights[j] = 1.0;

        for (int j = 0; j < ny.expected_keys.size(); ++j)
            if (ny.expected_keys[j] == "sector_target_y") ny.weights[j] = 1.0;
    }

    auto& vec = hunter_net.layer_2;
    std::fill(vec.weights_x.begin(), vec.weights_x.end(), 0.0);
    std::fill(vec.weights_y.begin(), vec.weights_y.end(), 0.0);
    vec.bias_x = vec.bias_y = 0.0;

    auto it_dx = std::find(vec.expected_keys_x.begin(), vec.expected_keys_x.end(), "dx");
    auto it_dy = std::find(vec.expected_keys_y.begin(), vec.expected_keys_y.end(), "dy");

    if (it_dx != vec.expected_keys_x.end())
        vec.weights_x[std::distance(vec.expected_keys_x.begin(), it_dx)] = 1.0;
    else
        std::cerr << "[Error] dx not found in expected_keys_x\n";

    if (it_dy != vec.expected_keys_y.end())
        vec.weights_y[std::distance(vec.expected_keys_y.begin(), it_dy)] = 1.0;
    else
        std::cerr << "[Error] dy not found in expected_keys_y\n";

    std::cout << "[Info] Hunter neural network fully scripted.\n";
}

void runMatchWithGUI(AgentControl control_mode,
                     Net_Hunter& hunter_net,
                     Net_Victim& victim_net,
                     std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                     const std::array<std::pair<double, double>, 4>& sector_centers,
                     EnvironmentGUI& gui,
                     const int statsValues[3])
{
    EnvironmentState state;
    state.runner_x = 100 + rand() % 600;
    state.runner_y = 100 + rand() % 400;
    state.chaser_x = 100 + rand() % 600;
    state.chaser_y = 100 + rand() % 400;

    hunter_net.setSectorPoints(&sector_points);
    victim_net.setSectorPoints(&sector_points);
    gui.setSectorPoints(&sector_points);

    const double max_speed = 2.0;
    int tripleStepEvery = 0;
    int agilityLevel = statsValues[2];

    if (agilityLevel == 2) tripleStepEvery = 5;
    else if (agilityLevel == 3) tripleStepEvery = 3;


    for (int step = 0; step < 500; ++step) {
        state.update_movement();

        int runner_sector = (state.runner_x > 400) + 2 * (state.runner_y > 300);
        int chaser_sector = (state.chaser_x > 400) + 2 * (state.chaser_y > 300);

        std::map<std::string, double> input = {
                {"runner_x", state.runner_x},
                {"runner_y", state.runner_y},
                {"chaser_x", state.chaser_x},
                {"chaser_y", state.chaser_y},
                {"runner_sector", static_cast<double>(runner_sector)},
                {"chaser_sector", static_cast<double>(chaser_sector)},
                {"distance_x", state.runner_x - state.chaser_x},
                {"distance_y", state.runner_y - state.chaser_y}
        };

        bool victimInBush = false;
        bool hunterInBush = false;
        const float bushRadius = 30.f;

        for (const auto& bush : bushPositions) {
            if (EnvUtils::distance(state.runner_x, state.runner_y, bush.x, bush.y) < bushRadius)
                victimInBush = true;
            if (EnvUtils::distance(state.chaser_x, state.chaser_y, bush.x, bush.y) < bushRadius)
                hunterInBush = true;
        }

        bool hunterCanSeeVictim = true;
        if (victimInBush && EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y) > 60.f)
            hunterCanSeeVictim = false;

        bool victimCanSeeHunter = !(hunterInBush && EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y) > 60.f);

        std::vector<double> hunter_move, victim_move;
        if (control_mode == AgentControl::NEURAL_VICTIM && statsValues[1] > 1) {
            input["in_bush"] = victimInBush ? 1.0 : 0.0;
            input["enemy_in_bush"] = hunterInBush ? 1.0 : 0.0;
            input["visible_enemy"] = hunterCanSeeVictim ? 1.0 : 0.0;

            sf::Vector2f nearest = bushPositions[0];
            float minDist = 99999.f;
            for (const auto& bush : bushPositions) {
                float d = EnvUtils::distance(state.runner_x, state.runner_y, bush.x, bush.y);
                if (d < minDist) {
                    minDist = d;
                    nearest = bush;
                }
            }
            input["nearest_bush_dx"] = nearest.x - state.runner_x;
            input["nearest_bush_dy"] = nearest.y - state.runner_y;
            input["bush_distance"] = minDist;
        }

        if (control_mode == AgentControl::NEURAL_HUNTER && statsValues[0] > 1) {
            input["in_bush"] = hunterInBush ? 1.0 : 0.0;
            input["enemy_in_bush"] = victimInBush ? 1.0 : 0.0;
            input["visible_enemy"] = victimCanSeeHunter ? 1.0 : 0.0;

            sf::Vector2f nearest = bushPositions[0];
            float minDist = 99999.f;
            for (const auto& bush : bushPositions) {
                float d = EnvUtils::distance(state.chaser_x, state.chaser_y, bush.x, bush.y);
                if (d < minDist) {
                    minDist = d;
                    nearest = bush;
                }
            }
            input["nearest_bush_dx"] = nearest.x - state.chaser_x;
            input["nearest_bush_dy"] = nearest.y - state.chaser_y;
            input["bush_distance"] = minDist;
        }


        switch (control_mode) {
            case AgentControl::NEURAL_HUNTER: {
                int hunter_sector = hunter_net.getChosenSector(input);
                input["sector_center_x"] = sector_centers[hunter_sector].first;
                input["sector_center_y"] = sector_centers[hunter_sector].second;
                hunter_move = hunter_net.process(input);
                victim_move = scriptedVictimMove(state);
                break;
            }
            case AgentControl::NEURAL_VICTIM: {
                int victim_sector = victim_net.getChosenSector(input);
                input["sector_center_x"] = sector_centers[victim_sector].first;
                input["sector_center_y"] = sector_centers[victim_sector].second;
                victim_move = victim_net.process(input);
                hunter_move = scriptedHunterMove(state);
                break;
            }
            case AgentControl::SCRIPTED_HUNTER: {
                hunter_move = scriptedHunterMove(state);
                victim_move = scriptedVictimMove(state);
                break;
            }
            case AgentControl::SCRIPTED_VICTIM: {
                int hunter_sector = hunter_net.getChosenSector(input);
                input["sector_center_x"] = sector_centers[hunter_sector].first;
                input["sector_center_y"] = sector_centers[hunter_sector].second;
                hunter_move = hunter_net.process(input);
                victim_move = scriptedVictimMove(state);
                break;
            }
        }


        state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
        state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);

        double runner_speed = max_speed;
        if (tripleStepEvery > 0 && (step % tripleStepEvery == 0))
            runner_speed = 6.0;

        double dx_v = victim_move[0] - state.runner_x;
        double dy_v = victim_move[1] - state.runner_y;
        auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
        state.runner_x += EnvUtils::clamp(norm_v[0] * runner_speed, -runner_speed, runner_speed);
        state.runner_y += EnvUtils::clamp(norm_v[1] * runner_speed, -runner_speed, runner_speed);

        state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
        state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
        state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
        state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);





        bool victimSmart = (control_mode == AgentControl::NEURAL_VICTIM && statsValues[1] > 1) ||
                           (control_mode == AgentControl::SCRIPTED_VICTIM && victimLevel != VictimDifficulty::EASY);

        bool hunterSmart = (control_mode == AgentControl::NEURAL_HUNTER && statsValues[0] > 1) ||
                           (control_mode == AgentControl::SCRIPTED_HUNTER && hunterLevel != HunterDifficulty::EASY);

        if (victimSmart && victimInBush && !hunterCanSeeVictim) {
            gui.runnerSprite.setColor(sf::Color(100, 255, 100));
        } else {
            gui.runnerSprite.setColor(sf::Color::White);
        }

        if (hunterSmart && hunterInBush && !victimCanSeeHunter) {
            gui.chaserSprite.setColor(sf::Color(100, 255, 100));
        } else {
            gui.chaserSprite.setColor(sf::Color::White);
        }



        gui.update(state);
        if (!gui.open()) break;

        double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        if (dist < 20.0) break;
    }
}

