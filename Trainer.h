// Trainer.h
#ifndef RL_GAME_TRAINER_H
#define RL_GAME_TRAINER_H

#include "Net_Hunter.h"
#include "Net_Victim.h"
#include "EnvironmentGUI.h"
#include "EnvironmentState.h"
#include "EnvUtils.h"
#include <map>
#include <array>
#include <vector>
#include <string>
#include <functional>

enum class AgentMode {
    RL_HUNTER,
    RL_VICTIM
};

double runEpisode(
        AgentMode mode,
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        std::array<std::pair<double, double>, 4> sector_centers,
        EnvironmentGUI& gui,
        int agilityLevel = 1,
        bool train_enabled = false,
        bool display_gui = false);


double runEpisode(
        AgentMode mode,
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        int agilityLevel,
        bool train_enabled,
        bool display_gui,
        std::function<void(EnvironmentState&, std::map<std::string, double>&)> augment_input);

double runEpisode_Hunter(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        std::array<std::pair<double, double>, 4> sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled = false,
        bool display_gui = false);


double runEpisode_Hunter(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled,
        bool display_gui,
        std::function<void(EnvironmentState&, std::map<std::string, double>&)> augment_input);

void trainAgent(
        AgentMode mode,
        int epochs,
        const std::string& save_path,
        bool show_gui_last = true);

double runEpisode_Intelligence_L2(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled = false,
        bool display_gui = false);

double runEpisode_Intelligence_L3(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled = false,
        bool display_gui = false);

double runEpisode_Intelligence_Hunter_L2(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled = false,
        bool display_gui = false);

double runEpisode_Intelligence_Hunter_L3(
        Net_Hunter& hunter_net,
        Net_Victim& victim_net,
        std::map<int, std::vector<std::pair<double, double>>>& sector_points,
        const std::vector<std::pair<double, double>>& sector_centers,
        EnvironmentGUI& gui,
        bool train_enabled = false,
        bool display_gui = false);



#endif // RL_GAME_TRAINER_H
