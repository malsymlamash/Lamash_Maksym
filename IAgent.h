//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_IAGENT_H
#define RL_GAME_IAGENT_H
class IAgent {
public:
//    virtual  std::map<std::string, double> decide(std::map<std::string,double>& input_map);
    virtual void giveReward(double reward) = 0;
    virtual void mutate(double mutation_power) = 0;
    virtual ~IAgent() = default;
    bool use_critic_learning = false;

};
#endif //RL_GAME_IAGENT_H
