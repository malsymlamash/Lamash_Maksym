//
// Created by 2005s on 18.05.2025.
//

#include "EnvUtils.h"

namespace EnvUtils {

    double distance(double x1, double y1, double x2, double y2) {
        return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    }

    std::vector<double> normalize_vector(double x, double y) {
        double length = std::sqrt(x * x + y * y);
        if (length == 0.0) return {0.0, 0.0};
        return {x / length, y / length};
    }

    double clamp(double val, double min_val, double max_val) {
        return std::max(min_val, std::min(max_val, val));
    }

}
