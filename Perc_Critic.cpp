//
// Created by 2005s on 18.05.2025.
//

#include "Perc_Critic.h"

void Perc_Critic::set_Reward(double reward) {
    given_reard = reward;
}

double Perc_Critic::averageValidWeight() const {
    double sum = 0.0;
    int count = 0;
    for (double w : weights) {
        if (std::isfinite(w) && std::abs(w) < 1e9) {
            sum += w;
            ++count;
        }
    }
    if (count == 0) {
        std::cerr << "[averageValidWeight] WARNING: No valid weights found. Returning 0.0\n";
        return 0.0;
    }
    return sum / count;
}

void Perc_Critic::learn(double learning_rate) {
    if (weights.size() != input_action.size()) {
        std::cerr << "[Critic] WARNING: Resizing weights from " << weights.size()
                  << " to " << input_action.size() << std::endl;
        weights.resize(input_action.size(), 0.0);
    }

    double error = given_reard - calculate();
    if (std::isnan(error) || std::isinf(error)) return;

    for (size_t i = 0; i < input_action.size(); ++i) {
        weights[i] += error * learning_rate * input_action[i];
        if (std::abs(weights[i]) > 1e9 || !std::isfinite(weights[i])) {
            weights[i] = averageValidWeight();
        }
    }
    bias += error * learning_rate;
    predicted_reward = calculate();
    input_action.clear();
}

double Perc_Critic::calculate() {
    if (weights.size() != input_action.size()) {
        std::cerr << "[FATAL] MISMATCH in Perc_Critic::calculate(): weights.size = "
                  << weights.size() << ", input_action.size = " << input_action.size() << std::endl;
        std::exit(1);
    }
    double sum = bias;
    for (size_t i = 0; i < weights.size(); ++i) {
        sum += weights[i] * input_action[i];
    }
    return sum;
}

void Perc_Critic::ensureInitialized(size_t expected_size) {
    if (weights.size() != expected_size) {
        weights.assign(expected_size, 1.0);
        bias = 0.0;
    }
}

void Perc_Critic::saveToStream(std::ofstream& out) const {
    out << bias << "\n";
    out << weights.size() << "\n";
    for (double w : weights) out << w << " ";
    out << "\n";
}

void Perc_Critic::loadFromStream(std::ifstream& in) {
    in >> bias;
    size_t size;
    in >> size;
    if (size == 0 || size > 1000 || !in.good()) {
        weights.assign(1, 1.0);
        bias = 0.0;
        return;
    }
    weights.resize(size);
    for (size_t i = 0; i < size; ++i) {
        if (!(in >> weights[i])) {
            weights.assign(size, 1.0);
            bias = 0.0;
            return;
        }
    }
}

