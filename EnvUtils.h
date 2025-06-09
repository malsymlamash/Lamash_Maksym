//
// Created by 2005s on 18.05.2025.
//

#ifndef RL_GAME_ENVUTILS_H
#define RL_GAME_ENVUTILS_H

#include <vector>
#include <cmath>
#include <algorithm>

namespace EnvUtils {
    double distance(double x1, double y1, double x2, double y2);
    std::vector<double> normalize_vector(double x, double y);
    double clamp(double val, double min_val, double max_val);
}

#endif //RL_GAME_ENVUTILS_H
