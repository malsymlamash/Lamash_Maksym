//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_SIMULATIONUTILS_H
#define RL_GAME_SIMULATIONUTILS_H
#pragma once
#include <vector>
#include <map>
#include <array>
#include "EnvironmentGUI.h"
#include "EnvironmentState.h"
#include "Net_Hunter.h"
#include "Net_Victim.h"

enum class AgentControl {
    NEURAL_HUNTER,
    NEURAL_VICTIM,
    SCRIPTED_HUNTER,
    SCRIPTED_VICTIM,
    RL_HUNTER,
    RL_VICTIM
};
enum class HunterDifficulty { EASY, MEDIUM, HARD };
enum class VictimDifficulty { EASY, MEDIUM, HARD };
extern HunterDifficulty hunterLevel;
extern VictimDifficulty victimLevel;
std::vector<double> scriptedHunterMove(const EnvironmentState& state);
std::vector<double> scriptedVictimMove(const EnvironmentState& state);

void setHunterToFullyScripted(Net_Hunter& hunter_net);

void runMatchWithGUI(AgentControl control_mode,
                     Net_Hunter& hunter_net,
                     Net_Victim& victim_net,
                     std::map<int, std::vector<std::pair<double, double>>>& sector_points,
                     const std::array<std::pair<double, double>, 4>& sector_centers,
                     EnvironmentGUI& gui,
                     const int statsValues[3]);

#endif //RL_GAME_SIMULATIONUTILS_H
