//
// Created by 2005s on 18.05.2025.
//

#include "EnvironmentState.h"

void EnvironmentState::update_movement() {
    run_dx = runner_x - prev_runner_x;
    run_dy = runner_y - prev_runner_y;
    chaser_dx = chaser_x - prev_chaser_x;
    chaser_dy = chaser_y - prev_chaser_y;

    prev_runner_x = runner_x;
    prev_runner_y = runner_y;
    prev_chaser_x = chaser_x;
    prev_chaser_y = chaser_y;
}
