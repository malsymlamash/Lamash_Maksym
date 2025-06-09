//
// Created by 2005s on 18.05.2025.
//

#include "Trainer.h"
#include "SimulationUtils.h"
#include "Net_Hunter.h"
#include "Net_Victim.h"
#include "EnvironmentGUI.h"
#include <map>
#include <array>
#include <string>
#include <functional>
#include "GUI.h"


double  runEpisode(AgentMode mode,
                   Net_Hunter& hunter_net,
                   Net_Victim& victim_net,
                   std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                   std::array<std::pair<double, double>, 4> sector_centers,
                   EnvironmentGUI& gui,
                   int agilityLevel,
                   bool train_enabled,
                   bool display_gui)
{
    EnvironmentState state;
    state.runner_x = 100 + rand() % 600;
    state.runner_y = 100 + rand() % 400;
    state.chaser_x = 100 + rand() % 600;
    state.chaser_y = 100 + rand() % 400;

    double reward_victim = 0.0;
    int victim_sector = 0;
    int step = 500;

    for (int i = 0; i < step; ++i) {
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
                {"distance_y", state.runner_y - state.chaser_y},
        };

        std::vector<double> hunter_move;
        std::vector<double> victim_move;

        if (mode == AgentMode::RL_VICTIM) {
            int sector_guess = 0;  // временно
            input["sector_center_x"] = sector_centers[sector_guess].first;
            input["sector_center_y"] = sector_centers[sector_guess].second;

            victim_sector = victim_net.getChosenSector(input);


            input["sector_center_x"] = sector_centers[victim_sector].first;
            input["sector_center_y"] = sector_centers[victim_sector].second;

            if (victim_net.sector_points_ptr == nullptr) {
                std::cerr << "[ERROR] sector_points_ptr is nullptr in victim_net!\n";
                std::exit(1);
            }
            if (!sector_points.contains(victim_sector)) {
                std::cerr << "[ERROR] No sector points for sector: " << victim_sector << "\n";
                std::exit(1);
            }


            victim_move = victim_net.process(input);
            hunter_move = scriptedHunterMove(state);
        }

        double max_speed = 2.0;
        int tripleStepEvery = 0;
        if (agilityLevel == 2) tripleStepEvery = 5;
        else if (agilityLevel == 3) tripleStepEvery = 3;

        double runner_speed = 2.0;
        if (tripleStepEvery > 0 && (i % tripleStepEvery == 0))
            runner_speed = 6.0;


        state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
        state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);


        double dx_v = victim_move[0] - state.runner_x;
        double dy_v = victim_move[1] - state.runner_y;

        auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
        state.runner_x += EnvUtils::clamp(norm_v[0] * runner_speed, -runner_speed, runner_speed);
        state.runner_y += EnvUtils::clamp(norm_v[1] * runner_speed, -runner_speed, runner_speed);

        state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
        state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
        state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
        state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);

        if (display_gui) {
            gui.update(state);
            if (!gui.open()) break;
        }

        double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double closeness = 1.0 / (dist + 1e-5);
        reward_victim -= closeness * 5.0;

        if (runner_sector != chaser_sector) reward_victim += 1.0;
        if (dist < 20.0) {
            reward_victim -= 20.0;
            break;
        }
        if (i >= step - 1) reward_victim += 25.0;
    }

    if (train_enabled) {
        victim_net.layer_0[victim_sector].giveReward(reward_victim);
        victim_net.layer_0[victim_sector].mutate(0.2);
        victim_net.layer_1_x[victim_sector].giveReward(reward_victim);
        victim_net.layer_1_x[victim_sector].mutate(0.2);
        victim_net.layer_1_y[victim_sector].giveReward(reward_victim);
        victim_net.layer_1_y[victim_sector].mutate(0.2);

        double dist_from_chaser = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double vec_reward = std::clamp(dist_from_chaser / 800.0, 0.0, 1.0);
        victim_net.layer_2.giveReward(vec_reward);
        victim_net.layer_2.mutate(0.2);
    }

    return reward_victim;
}





double runEpisode(AgentMode mode,
                  Net_Hunter& hunter_net,
                  Net_Victim& victim_net,
                  std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                  const std::vector<std::pair<double, double>>& sector_centers,
                  EnvironmentGUI& gui,
                  int agilityLevel,
                  bool train_enabled,
                  bool display_gui,
                  std::function<void(EnvironmentState&, std::map<std::string, double>&)> augment_input)
{
    EnvironmentState state;
    state.runner_x = 100 + rand() % 600;
    state.runner_y = 100 + rand() % 400;
    state.chaser_x = 100 + rand() % 600;
    state.chaser_y = 100 + rand() % 400;

    double reward_victim = 0.0;
    int victim_sector = 0;
    int step = 500;

    for (int i = 0; i < step; ++i) {
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
                {"distance_y", state.runner_y - state.chaser_y},
        };

        augment_input(state, input);

        std::vector<double> hunter_move;
        std::vector<double> victim_move;

        if (mode == AgentMode::RL_VICTIM) {
            int sector_guess = 0;
            input["sector_center_x"] = sector_centers[sector_guess].first;
            input["sector_center_y"] = sector_centers[sector_guess].second;

            victim_sector = victim_net.getChosenSector(input);
            input["sector_center_x"] = sector_centers[victim_sector].first;
            input["sector_center_y"] = sector_centers[victim_sector].second;

            if (victim_net.sector_points_ptr == nullptr) {
                std::cerr << "[ERROR] sector_points_ptr is nullptr in victim_net!\n";
                std::exit(1);
            }
            if (!sector_points.contains(victim_sector)) {
                std::cerr << "[ERROR] No sector points for sector: " << victim_sector << "\n";
                std::exit(1);
            }

            victim_move = victim_net.process(input);
            hunter_move = scriptedHunterMove(state);
        }

        double max_speed = 2.0;
        int tripleStepEvery = 0;
        if (agilityLevel == 2) tripleStepEvery = 5;
        else if (agilityLevel == 3) tripleStepEvery = 3;

        double runner_speed = (tripleStepEvery > 0 && (i % tripleStepEvery == 0)) ? 6.0 : 2.0;

        state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
        state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);

        double dx_v = victim_move[0] - state.runner_x;
        double dy_v = victim_move[1] - state.runner_y;

        auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
        state.runner_x += EnvUtils::clamp(norm_v[0] * runner_speed, -runner_speed, runner_speed);
        state.runner_y += EnvUtils::clamp(norm_v[1] * runner_speed, -runner_speed, runner_speed);

        state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
        state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
        state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
        state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);

        if (display_gui) {
            gui.update(state);
            if (!gui.open()) break;
        }

        double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double closeness = 1.0 / (dist + 1e-5);
        reward_victim -= closeness * 5.0;

        if (runner_sector != chaser_sector) reward_victim += 1.0;
        if (dist < 20.0) {
            reward_victim -= 20.0;
            break;
        }
        if (i >= step - 1) reward_victim += 25.0;
    }

    if (train_enabled) {
        victim_net.layer_0[victim_sector].giveReward(reward_victim);
        victim_net.layer_0[victim_sector].mutate(0.2);
        victim_net.layer_1_x[victim_sector].giveReward(reward_victim);
        victim_net.layer_1_x[victim_sector].mutate(0.2);
        victim_net.layer_1_y[victim_sector].giveReward(reward_victim);
        victim_net.layer_1_y[victim_sector].mutate(0.2);

        double dist_from_chaser = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double vec_reward = std::clamp(dist_from_chaser / 800.0, 0.0, 1.0);
        victim_net.layer_2.giveReward(vec_reward);
        victim_net.layer_2.mutate(0.2);
    }

    return reward_victim;
}

double runEpisode_Hunter(Net_Hunter& hunter_net,
                         Net_Victim& victim_net,
                         std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                         std::array<std::pair<double, double>, 4> sector_centers,
                         EnvironmentGUI& gui,
                         bool train_enabled,
                         bool display_gui)
{
    EnvironmentState state;
    state.runner_x = 100 + rand() % 600;
    state.runner_y = 100 + rand() % 400;
    state.chaser_x = 100 + rand() % 600;
    state.chaser_y = 100 + rand() % 400;

    double reward_hunter = 0.0;
    int hunter_sector = 0;
    int step = 500;

    for (int i = 0; i < step; ++i) {
        state.update_movement();

        int runner_sector = (state.runner_x > 400) + 2 * (state.runner_y > 300);
        int chaser_sector = (state.chaser_x > 400) + 2 * (state.chaser_y > 300);

// 1. Сначала базовый input
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


        std::vector<double> hunter_move;
        std::vector<double> victim_move;

        hunter_sector = hunter_net.getChosenSector(input);

        input["sector_center_x"] = sector_centers[hunter_sector].first;
        input["sector_center_y"] = sector_centers[hunter_sector].second;

        hunter_move = hunter_net.process(input);
        victim_move = scriptedVictimMove(state);

        double max_speed = 2.0;

        state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
        state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);

        double dx_v = victim_move[0] - state.runner_x;
        double dy_v = victim_move[1] - state.runner_y;
        auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
        state.runner_x += EnvUtils::clamp(norm_v[0] * max_speed, -max_speed, max_speed);
        state.runner_y += EnvUtils::clamp(norm_v[1] * max_speed, -max_speed, max_speed);

        state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
        state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
        state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
        state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);

        if (display_gui) {
            gui.update(state);
            if (!gui.open()) break;
        }

        double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double closeness = 1.0 / (dist + 1e-5);
        reward_hunter += closeness * 15.0;

        if (runner_sector == chaser_sector) reward_hunter += 3.0;
        else reward_hunter -= 1.5;

        if (dist < 20.0) {
            reward_hunter += 20.0;
            break;
        }
        if (i >= step - 1) reward_hunter -= 10.0;
    }

    if (train_enabled) {
        hunter_net.layer_0[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_0[hunter_sector].mutate(0.2);
        hunter_net.layer_1_x[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_1_x[hunter_sector].mutate(0.2);
        hunter_net.layer_1_y[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_1_y[hunter_sector].mutate(0.2);

        double dist_to_target = EnvUtils::distance(state.chaser_x, state.chaser_y, state.runner_x, state.runner_y);
        double vec_reward = std::clamp((800.0 - dist_to_target) / 800.0, 0.0, 1.0);
        hunter_net.layer_2.giveReward(vec_reward);
        hunter_net.layer_2.mutate(0.2);
    }

    return reward_hunter;
}
double runEpisode_Hunter(Net_Hunter& hunter_net,
                         Net_Victim& victim_net,
                         std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                         const std::vector<std::pair<double, double>>& sector_centers,
                         EnvironmentGUI& gui,
                         bool train_enabled,
                         bool display_gui,
                         std::function<void(EnvironmentState&, std::map<std::string, double>&)> augment_input)
{
    EnvironmentState state;
    state.runner_x = 100 + rand() % 600;
    state.runner_y = 100 + rand() % 400;
    state.chaser_x = 100 + rand() % 600;
    state.chaser_y = 100 + rand() % 400;

    double reward_hunter = 0.0;
    int hunter_sector = 0;
    int step = 500;

    for (int i = 0; i < step; ++i) {
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

        // 💡 ВСТАВКА: дополнительные признаки
        augment_input(state, input);

        std::vector<double> hunter_move;
        std::vector<double> victim_move;

        hunter_sector = hunter_net.getChosenSector(input);

        input["sector_center_x"] = sector_centers[hunter_sector].first;
        input["sector_center_y"] = sector_centers[hunter_sector].second;

        hunter_move = hunter_net.process(input);
        victim_move = scriptedVictimMove(state);

        double max_speed = 2.0;

        state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
        state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);

        double dx_v = victim_move[0] - state.runner_x;
        double dy_v = victim_move[1] - state.runner_y;
        auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
        state.runner_x += EnvUtils::clamp(norm_v[0] * max_speed, -max_speed, max_speed);
        state.runner_y += EnvUtils::clamp(norm_v[1] * max_speed, -max_speed, max_speed);

        state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
        state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
        state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
        state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);

        if (display_gui) {
            gui.update(state);
            if (!gui.open()) break;
        }

        double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
        double closeness = 1.0 / (dist + 1e-5);
        reward_hunter += closeness * 15.0;

        if (runner_sector == chaser_sector) reward_hunter += 3.0;
        else reward_hunter -= 1.5;

        if (dist < 20.0) {
            reward_hunter += 20.0;
            break;
        }
        if (i >= step - 1) reward_hunter -= 10.0;
    }

    if (train_enabled) {
        hunter_net.layer_0[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_0[hunter_sector].mutate(0.2);
        hunter_net.layer_1_x[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_1_x[hunter_sector].mutate(0.2);
        hunter_net.layer_1_y[hunter_sector].giveReward(reward_hunter);
        hunter_net.layer_1_y[hunter_sector].mutate(0.2);

        double dist_to_target = EnvUtils::distance(state.chaser_x, state.chaser_y, state.runner_x, state.runner_y);
        double vec_reward = std::clamp((800.0 - dist_to_target) / 800.0, 0.0, 1.0);
        hunter_net.layer_2.giveReward(vec_reward);
        hunter_net.layer_2.mutate(0.2);
    }

    return reward_hunter;
}

//Defoult method to train 1,1,1 lvels
void trainAgent(AgentMode mode,
                int epochs,
                const std::string& save_path,
                bool show_gui_last)
{
    EnvironmentGUI gui;
    Net_Hunter hunter_net(4);
    Net_Victim victim_net(4);

    std::map<int, std::vector<std::pair<double, double>>> sector_points;
    std::array<std::pair<double, double>, 4> sector_centers = {
            std::make_pair(200.0, 150.0),
            std::make_pair(600.0, 150.0),
            std::make_pair(200.0, 450.0),
            std::make_pair(600.0, 450.0)
    };

    for (int sector = 0; sector < 4; ++sector) {
        std::vector<std::pair<double, double>> points;
        double base_x = (sector % 2 == 0) ? 0 : 400;
        double base_y = (sector / 2 == 0) ? 0 : 300;
        for (int i = 1; i <= 5; ++i)
            for (int j = 1; j <= 4; ++j)
                points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
        sector_points[sector] = points;
    }

    hunter_net.setSectorPoints(&sector_points);
    victim_net.setSectorPoints(&sector_points);
    gui.setSectorPoints(&sector_points);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        double reward = 0.0;
        if (mode == AgentMode::RL_VICTIM) {
            reward = runEpisode(AgentMode::RL_VICTIM, hunter_net, victim_net,
                                sector_points, sector_centers, gui,
                                true, show_gui_last && epoch >= epochs - 10);
        } else if (mode == AgentMode::RL_HUNTER) {
            reward = runEpisode_Hunter(hunter_net, victim_net,
                                       sector_points, sector_centers, gui,
                                       true, show_gui_last && epoch >= epochs - 10);
        }

        if (epoch % 100 == 0) {
            std::cout << "[Epoch " << epoch << "] "
                      << (mode == AgentMode::RL_VICTIM ? "Victim" : "Hunter")
                      << " reward: " << reward << std::endl;
        }
    }

    if (mode == AgentMode::RL_VICTIM)
        victim_net.saveToFile(save_path);
    else if (mode == AgentMode::RL_HUNTER)
        hunter_net.saveToFile(save_path);
}



double runEpisode_Intelligence_L2(Net_Hunter& hunter_net, Net_Victim& victim_net,
                                  std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                                  const std::vector<std::pair<double, double>>& sector_centers,
                                  EnvironmentGUI& gui,
                                  bool train_enabled,
                                  bool display_gui) {

    auto bushes = gui.getBushPositions();

    return runEpisode(AgentMode::RL_VICTIM, hunter_net, victim_net, sector_points, sector_centers, gui, 1, train_enabled, display_gui,
                      [&](EnvironmentState& state, std::map<std::string, double>& input) {
                          sf::Vector2f runner(state.runner_x, state.runner_y);
                          float bestDist = 10000;
                          sf::Vector2f closest;

                          for (auto& bush : bushes) {
                              float dist = EnvUtils::distance(runner.x, runner.y, bush.x, bush.y);
                              if (dist < bestDist) {
                                  bestDist = dist;
                                  closest = bush;
                              }
                          }

                          input["nearest_bush_dx"] = closest.x - runner.x;
                          input["nearest_bush_dy"] = closest.y - runner.y;
                      });
}


double runEpisode_Intelligence_L3(Net_Hunter& hunter_net, Net_Victim& victim_net,
                                  std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                                  std::vector<std::pair<double, double>> sector_centers,
                                  EnvironmentGUI& gui,
                                  bool train_enabled,
                                  bool display_gui) {
    auto bushes = gui.getBushPositions();

    return runEpisode(AgentMode::RL_VICTIM, hunter_net, victim_net, sector_points, sector_centers, gui, 1, train_enabled, display_gui,
                      [&](EnvironmentState& state, std::map<std::string, double>& input) {
                          float minDist = 10000;
                          bool in_bush = false;

                          for (auto& bush : bushes) {
                              float dist = EnvUtils::distance(state.runner_x, state.runner_y, bush.x, bush.y);
                              if (dist < 30.0) in_bush = true;
                              if (dist < minDist) {
                                  minDist = dist;
                                  input["nearest_bush_dx"] = bush.x - state.runner_x;
                                  input["nearest_bush_dy"] = bush.y - state.runner_y;
                              }
                          }

                          input["in_bush"] = in_bush ? 1.0 : 0.0;
                      });
}

double runEpisode_Intelligence_Hunter_L2(Net_Hunter& hunter_net, Net_Victim& victim_net,
                                         std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                                         const std::vector<std::pair<double, double>>& sector_centers,
                                         EnvironmentGUI& gui,
                                         bool train_enabled,
                                         bool display_gui) {
    auto bushes = gui.getBushPositions();

    return runEpisode_Hunter(hunter_net, victim_net, sector_points, sector_centers, gui, train_enabled, display_gui,
                             [&](EnvironmentState& state, std::map<std::string, double>& input) {
                                 float bestDist = 10000;
                                 sf::Vector2f bestBush;

                                 for (auto& bush : bushes) {
                                     float dist = EnvUtils::distance(state.chaser_x, state.chaser_y, bush.x, bush.y);
                                     if (dist < bestDist) {
                                         bestDist = dist;
                                         bestBush = bush;
                                     }
                                 }

                                 bool sees_victim = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y) < 200.0;
                                 input["nearest_bush_dx"] = bestBush.x - state.chaser_x;
                                 input["nearest_bush_dy"] = bestBush.y - state.chaser_y;
                                 input["visible_enemy"] = sees_victim ? 1.0 : 0.0;
                             });
}

double runEpisode_Intelligence_Hunter_L3(Net_Hunter& hunter_net, Net_Victim& victim_net,
                                         std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                                         const std::vector<std::pair<double, double>>& sector_centers,
                                         EnvironmentGUI& gui,
                                         bool train_enabled,
                                         bool display_gui) {
    auto bushes = gui.getBushPositions();

    return runEpisode_Hunter(hunter_net, victim_net, sector_points, sector_centers, gui, train_enabled, display_gui,[&](EnvironmentState& state, std::map<std::string, double>& input) {bool in_bush = false;sf::Vector2f best;
                                 for (auto& bush : bushes) {
                                     float dist = EnvUtils::distance(state.chaser_x, state.chaser_y, bush.x, bush.y);
                                     if (dist < 30.0) in_bush = true;
                                     if (dist < EnvUtils::distance(state.chaser_x, state.chaser_y, best.x, best.y))
                                         best = bush;
                                 }

                                 bool sees_victim = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y) < 200.0;
                                 input["in_bush"] = in_bush ? 1.0 : 0.0;
                                 input["visible_enemy"] = sees_victim ? 1.0 : 0.0;
                                 input["nearest_bush_dx"] = best.x - state.chaser_x;
                                 input["nearest_bush_dy"] = best.y - state.chaser_y;
                             });
}


void trainVictimIntelligence_Level2(int epochs, const std::string& save_path) {
    EnvironmentGUI gui;
    Net_Victim victim_net(4);
    Net_Hunter hunter_net(4);

    std::map<int, std::vector<std::pair<double, double>>> sector_points;
    std::vector<std::pair<double, double>> sector_centers = {
            std::make_pair(200.0, 150.0), std::make_pair(600.0, 150.0),
            std::make_pair(200.0, 450.0), std::make_pair(600.0, 450.0)};

    for (int sector = 0; sector < 4; ++sector) {
        std::vector<std::pair<double, double>> points;
        double base_x = (sector % 2 == 0) ? 0 : 400;
        double base_y = (sector / 2 == 0) ? 0 : 300;
        for (int i = 1; i <= 5; ++i)
            for (int j = 1; j <= 4; ++j)
                points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
        sector_points[sector] = points;
    }

    victim_net.setSectorPoints(&sector_points);
    gui.setSectorPoints(&sector_points);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        double reward = runEpisode_Intelligence_L2(hunter_net, victim_net, sector_points, sector_centers, gui, true, epoch >= epochs - 10);

        if (epoch % 100 == 0)
            std::cout << "[Victim Int2] Epoch " << epoch << " reward: " << reward << std::endl;
    }
    victim_net.saveToFile(save_path);
}


//------Train victim Intilij 3 broken(
//
//void trainVictimIntelligence_Level3(int epochs, const std::string& save_path) {
//    EnvironmentGUI gui;
//    Net_Victim victim_net(4);
//    Net_Hunter hunter_net(4);
//
//    std::map<int, std::vector<std::pair<double, double>>> sector_points;
//    std::vector<std::pair<double, double>> sector_centers = {
//            std::make_pair(200.0, 150.0), std::make_pair(600.0, 150.0),
//            std::make_pair(200.0, 450.0), std::make_pair(600.0, 450.0)};
//
//    for (int sector = 0; sector < 4; ++sector) {
//        std::vector<std::pair<double, double>> points;
//        double base_x = (sector % 2 == 0) ? 0 : 400;
//        double base_y = (sector / 2 == 0) ? 0 : 300;
//        for (int i = 1; i <= 5; ++i)
//            for (int j = 1; j <= 4; ++j)
//                points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
//        sector_points[sector] = points;
//    }
//
//    victim_net.setSectorPoints(&sector_points);
//    gui.setSectorPoints(&sector_points);
//
//    for (int epoch = 0; epoch < epochs; ++epoch) {
//        double reward = runEpisode_Intelligence_L3(hunter_net, victim_net, sector_points, sector_centers, gui, true, epoch >= epochs - 10);
//
//        if (epoch % 100 == 0)
//            std::cout << "[Victim Int3] Epoch " << epoch << " reward: " << reward << std::endl;
//    }
//    victim_net.saveToFile(save_path);
//}


void trainHunterIntelligence_Level2(int epochs, const std::string& save_path) {
    EnvironmentGUI gui;
    Net_Hunter hunter_net(4);
    Net_Victim victim_net(4);

    std::map<int, std::vector<std::pair<double, double>>> sector_points;
    std::vector<std::pair<double, double>> sector_centers = {
            std::make_pair(200.0, 150.0), std::make_pair(600.0, 150.0),
            std::make_pair(200.0, 450.0), std::make_pair(600.0, 450.0)};

    for (int sector = 0; sector < 4; ++sector) {
        std::vector<std::pair<double, double>> points;
        double base_x = (sector % 2 == 0) ? 0 : 400;
        double base_y = (sector / 2 == 0) ? 0 : 300;
        for (int i = 1; i <= 5; ++i)
            for (int j = 1; j <= 4; ++j)
                points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
        sector_points[sector] = points;
    }

    hunter_net.setSectorPoints(&sector_points);
    gui.setSectorPoints(&sector_points);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        double reward = runEpisode_Intelligence_Hunter_L2(hunter_net, victim_net, sector_points, sector_centers, gui, true, epoch >= epochs - 10);

        if (epoch % 100 == 0)
            std::cout << "[Hunter Int2] Epoch " << epoch << " reward: " << reward << std::endl;
    }
    hunter_net.saveToFile(save_path);
}

void trainHunterIntelligence_Level3(int epochs, const std::string& save_path) {
    EnvironmentGUI gui;
    Net_Hunter hunter_net(4);
    Net_Victim victim_net(4);

    std::map<int, std::vector<std::pair<double, double>>> sector_points;
    std::vector<std::pair<double, double>> sector_centers = {
            std::make_pair(200.0, 150.0), std::make_pair(600.0, 150.0),
            std::make_pair(200.0, 450.0), std::make_pair(600.0, 450.0)};

    for (int sector = 0; sector < 4; ++sector) {
        std::vector<std::pair<double, double>> points;
        double base_x = (sector % 2 == 0) ? 0 : 400;
        double base_y = (sector / 2 == 0) ? 0 : 300;
        for (int i = 1; i <= 5; ++i)
            for (int j = 1; j <= 4; ++j)
                points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
        sector_points[sector] = points;
    }

    hunter_net.setSectorPoints(&sector_points);
    gui.setSectorPoints(&sector_points);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        double reward = runEpisode_Intelligence_Hunter_L3(hunter_net, victim_net, sector_points, sector_centers, gui, true, epoch >= epochs - 10);


        if (epoch % 100 == 0)
            std::cout << "[Hunter Int3] Epoch " << epoch << " reward: " << reward << std::endl;
    }
    hunter_net.saveToFile(save_path);
}


//---train sectors 2 lvl broken((
//void trainSectorAwareness_Level2(AgentMode mode, int epochs, const std::string& save_path) {
//    int sector_count = 9;
//    EnvironmentGUI gui;
//    Net_Hunter hunter_net(sector_count);
//    Net_Victim victim_net(sector_count);
//
//    std::map<int, std::vector<std::pair<double, double>>> sector_points;
//    std::array<std::pair<double, double>, 9> centers;
//    int index = 0;
//    for (int i = 0; i < 3; ++i)
//        for (int j = 0; j < 3; ++j)
//            centers[index++] = { 133.3 + j * 266.6, 100.0 + i * 200.0 };
//
//    for (int s = 0; s < sector_count; ++s) {
//        double base_x = (s % 3) * 266.6;
//        double base_y = (s / 3) * 200.0;
//        std::vector<std::pair<double, double>> pts;
//        for (int i = 1; i <= 3; ++i)
//            for (int j = 1; j <= 3; ++j)
//                pts.emplace_back(base_x + i * 266.6 / 4.0, base_y + j * 200.0 / 4.0);
//        sector_points[s] = pts;
//    }
//
//    hunter_net.setSectorPoints(&sector_points);
//    victim_net.setSectorPoints(&sector_points);
//    gui.setSectorPoints(&sector_points);
//
//    for (int epoch = 0; epoch < epochs; ++epoch) {
//        double reward = (mode == AgentMode::RL_VICTIM)
//                        ? runEpisode(mode, hunter_net, victim_net, sector_points, centers, gui, 1, true)
//                        : runEpisode_Hunter(hunter_net, victim_net, sector_points, centers, gui, true);
//
//        if (epoch % 100 == 0)
//            std::cout << "[Sector 9] Epoch " << epoch << ": " << reward << std::endl;
//    }
//
//    if (mode == AgentMode::RL_VICTIM) victim_net.saveToFile(save_path);
//    else hunter_net.saveToFile(save_path);
//}

//---train sectors 3 lvl broken((
//void trainSectorAwareness_Level3(AgentMode mode, int epochs, const std::string& save_path) {
//    int sector_count = 16;
//    EnvironmentGUI gui;
//    Net_Hunter hunter_net(sector_count);
//    Net_Victim victim_net(sector_count);
//
//    std::map<int, std::vector<std::pair<double, double>>> sector_points;
//    std::array<std::pair<double, double>, 9> centers;
//    int index = 0;
//    for (int i = 0; i < 4; ++i)
//        for (int j = 0; j < 4; ++j)
//            centers[index++] = { 100.0 + j * 200.0, 75.0 + i * 150.0 };
//
//    for (int s = 0; s < sector_count; ++s) {
//        double base_x = (s % 4) * 200.0;
//        double base_y = (s / 4) * 150.0;
//        std::vector<std::pair<double, double>> pts;
//        for (int i = 1; i <= 3; ++i)
//            for (int j = 1; j <= 3; ++j)
//                pts.emplace_back(base_x + i * 200.0 / 4.0, base_y + j * 150.0 / 4.0);
//        sector_points[s] = pts;
//    }
//
//    hunter_net.setSectorPoints(&sector_points);
//    victim_net.setSectorPoints(&sector_points);
//    gui.setSectorPoints(&sector_points);
//
//    for (int epoch = 0; epoch < epochs; ++epoch) {
//        double reward = (mode == AgentMode::RL_VICTIM)
//                        ? runEpisode(mode, hunter_net, victim_net, sector_points, centers, gui, 1, true)
//                        : runEpisode_Hunter(hunter_net, victim_net, sector_points, centers, gui, true);
//
//        if (epoch % 100 == 0)
//            std::cout << "[Sector 16] Epoch " << epoch << ": " << reward << std::endl;
//    }
//
//    if (mode == AgentMode::RL_VICTIM) victim_net.saveToFile(save_path);
//    else hunter_net.saveToFile(save_path);
//}
