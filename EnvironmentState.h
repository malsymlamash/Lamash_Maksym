//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_ENVIRONMENT_STATE_H
#define RL_GAME_ENVIRONMENT_STATE_H

struct EnvironmentState {
    double runner_x = 400, runner_y = 300;
    double chaser_x = 600, chaser_y = 400;

    double prev_runner_x = runner_x, prev_runner_y = runner_y;
    double prev_chaser_x = chaser_x, prev_chaser_y = chaser_y;

    double run_dx = 0, run_dy = 0;
    double chaser_dx = 0, chaser_dy = 0;

    void update_movement();
};

#endif // RL_GAME_ENVIRONMENT_STATE_H
