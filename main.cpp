#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include "GUI.h"


//--------------------------------------TRAIN poputki buli(
//int main() {
//    EnvironmentGUI gui;
//    EnvironmentState state;
//
//    Net_Hunter hunter_net(4);
//    Net_Victim victim_net(4);
//
//    Net_Victim frozen_victim = victim_net;
//    Net_Hunter frozen_hunter = hunter_net;
//
//
//    double best_reward_hunter = -1e9;
//    double best_reward_victim = -1e9;
//
//    Net_Hunter best_hunter = hunter_net;
//    Net_Victim best_victim = victim_net;
//
//    bool train_hunter = false;
//    bool train_victim = false;
//
//
//
//    // Проверяем наличие файлов
//    std::ifstream hunter_check("hunter_weights_with_critic.txt");
//    if (hunter_check.good()) {
//        best_hunter.loadFromFile("hunter_weights_with_critic.txt");
//        frozen_hunter = best_hunter;
//    }
//    hunter_check.close();
//
//    std::ifstream victim_check("victim_weights_with_critic.txt");
//    if (victim_check.good()) {
//        best_victim.loadFromFile("victim_weights_with_critic.txt");
//        frozen_victim = best_victim;
//    }
//    victim_check.close();
//
//
//
//    std::map<int, std::vector<std::pair<double, double>>> sector_points;
//
//    auto generateSectorPoints = [&]() {
//        for (int sector = 0; sector < 4; ++sector) {
//            std::vector<std::pair<double, double>> points;
//            double base_x = (sector % 2 == 0) ? 0 : 400;
//            double base_y = (sector / 2 == 0) ? 0 : 300;
//
//            for (int i = 1; i <= 5; ++i) {
//                for (int j = 1; j <= 4; ++j) {
//                    double x = base_x + i * 400.0 / 6.0;
//                    double y = base_y + j * 300.0 / 5.0;
//                    points.emplace_back(x, y);
//                }
//            }
//            sector_points[sector] = points;
//        }
//    };
//
//
//    auto getSector = [](double x, double y) -> int {
//        bool right = x > 400;
//        bool bottom = y > 300;
//        return bottom * 2 + right;
//    };
//
//    std::array<std::pair<double, double>, 4> sector_centers = {
//            std::make_pair(200.0, 150.0), // 0
//            std::make_pair(600.0, 150.0), // 1
//            std::make_pair(200.0, 450.0), // 2
//            std::make_pair(600.0, 450.0)  // 3
//    };
//
//    generateSectorPoints();
//
//    hunter_net.setSectorPoints(&sector_points);
//    victim_net.setSectorPoints(&sector_points);
//
//    frozen_victim.setSectorPoints(&sector_points);
//    frozen_hunter.setSectorPoints(&sector_points);
//
//    gui.setSectorPoints(&sector_points);
//
//
//
//
//
//    for (int epoch = 0; epoch < 3100; ++epoch) {
//
//        train_hunter = false;
//        train_victim = false;
//
//
//        if (epoch == 1000) {
//            hunter_net.setCriticLearning(true);
//            victim_net.setCriticLearning(true);
//        }
//
//        if (epoch % 2 == 0) {
//            train_hunter = true;
//        }else {
//            train_victim = true;
//        }
//
//        // === RANDOM SPAWN ===
//        state.runner_x = 100 + rand() % 600;
//        state.runner_y = 100 + rand() % 400;
//        state.chaser_x = 100 + rand() % 600;
//        state.chaser_y = 100 + rand() % 400;
//
//        double reward_hunter = 0.0;
//        double reward_victim = 0.0;
//
//        int hunter_sector = 0;
//        int victim_sector = 0;
//
////        int step = (epoch < 2000) ? 500 : 2000;
//        int step = 500;
//
//        std::vector<double> hunter_move;
//        std::vector<double> victim_move;
//
//
//        for (int i = 0; i < step; ++i) {
//            state.update_movement();
//
//            int runner_sector = getSector(state.runner_x, state.runner_y);
//            int chaser_sector = getSector(state.chaser_x, state.chaser_y);
//
//            std::map<std::string, double> input = {
//                    {"runner_x", state.runner_x},
//                    {"runner_y", state.runner_y},
//                    {"chaser_x", state.chaser_x},
//                    {"chaser_y", state.chaser_y},
//                    {"runner_sector", static_cast<double>(runner_sector)},
//                    {"chaser_sector", static_cast<double>(chaser_sector)},
//                    {"distance_x",state.runner_x - state.chaser_x},
//                    {"distance_y", state.runner_y - state.chaser_y}
//
//            };
//            hunter_sector = hunter_net.getChosenSector(input);
//            input["sector_center_x"] = sector_centers[hunter_sector].first;
//            input["sector_center_y"] = sector_centers[hunter_sector].second;
//
//            int frozen_hunter_sector = frozen_hunter.getChosenSector(input);
//
//            victim_sector = victim_net.getChosenSector(input);
//            input["sector_center_x"] = sector_centers[victim_sector].first;
//            input["sector_center_y"] = sector_centers[victim_sector].second;
//
//            int frozen_victim_sector = frozen_victim.getChosenSector(input);
//
//
//
//            // === ВЫБОР СЕКТОРА ЖЕРТВОЙ ===
//            double max_speed = 2.0;
////---Working sektors
////            auto [vcx, vcy] = sector_centers[victim_sector];
////            state.runner_x += EnvUtils::clamp(vcx - state.runner_x, -max_speed, max_speed);
////            state.runner_y += EnvUtils::clamp(vcy - state.runner_y, -max_speed, max_speed);
////
//////            std::cout << "[Victim] Chose sector: " << victim_sector << " -> (" << vcx << ", " << vcy << ")\n";
////
////            auto [hcx, hcy] = sector_centers[hunter_sector];
////            state.chaser_x += EnvUtils::clamp(hcx - state.chaser_x, -max_speed, max_speed);
////            state.chaser_y += EnvUtils::clamp(hcy - state.chaser_y, -max_speed, max_speed);
////            auto hunter_move = hunter_net.process(input);
////            auto victim_move = victim_net.process(input);
//
////            auto hunter_move = (epoch >= 2000)
////                               ? best_hunter.process(input)
////                               : (train_hunter ? hunter_net.process(input) : frozen_hunter.process(input));
////
////            auto victim_move = (epoch >= 2000)
////                               ? best_victim.process(input)
////                               : (train_victim ? victim_net.process(input) : frozen_victim.process(input));
//                hunter_move = train_hunter
//                               ? hunter_net.process(input)
//                               : frozen_hunter.process(input);
//
//                victim_move = train_victim
//                               ? victim_net.process(input)
//                               : frozen_victim.process(input);
//
//
//            if (hunter_move.size() < 2 || victim_move.size() < 2) {
//                std::cerr << "Ошибка: hunter_move или victim_move недостаточной длины\n";
//                std::cerr << "hunter_move.size(): " << hunter_move.size() << ", victim_move.size(): " << victim_move.size() << std::endl;
//                break;
//            }
//
////            double cx_v = hunter_move[0] - state.chaser_x;
////            double cy_v = hunter_move[1] - state.chaser_y;
////            auto norm_c = EnvUtils::normalize_vector(cx_v, cy_v);
////            state.chaser_x += EnvUtils::clamp(norm_c[0] * max_speed, -max_speed, max_speed);
////            state.chaser_y += EnvUtils::clamp(norm_c[1] * max_speed, -max_speed, max_speed);
//            state.chaser_x += EnvUtils::clamp(hunter_move[0], -max_speed, max_speed);
//            state.chaser_y += EnvUtils::clamp(hunter_move[1], -max_speed, max_speed);
//
//
//
//            double dx_v = victim_move[0] - state.runner_x;
//            double dy_v = victim_move[1] - state.runner_y;
//            auto norm_v = EnvUtils::normalize_vector(dx_v, dy_v);
//            state.runner_x += EnvUtils::clamp(norm_v[0] * max_speed, -max_speed, max_speed);
//            state.runner_y += EnvUtils::clamp(norm_v[1] * max_speed, -max_speed, max_speed);
//
//
//
//            state.runner_x = EnvUtils::clamp(state.runner_x, 20.0, 780.0);
//            state.runner_y = EnvUtils::clamp(state.runner_y, 20.0, 580.0);
//            state.chaser_x = EnvUtils::clamp(state.chaser_x, 20.0, 780.0);
//            state.chaser_y = EnvUtils::clamp(state.chaser_y, 20.0, 580.0);
//
////            std::cout << "[Hunter] Chose sector: " << hunter_sector << " -> (" << hcx << ", " << hcy << ")\n";
//
//            // === GUI ===
//            if (epoch >= 3090) {
//                gui.update(state);
//                if (!gui.open()){
//                    best_hunter.saveToFile("hunter_weights_with_critic.txt");
//                    best_victim.saveToFile("victim_weights_with_critic.txt");
//                    std::cout << "[Info] Weights saved due to GUI close or final epoch.\n";
//                }
//            }
//            //------------Rewards System suka
//            double dist = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
//            double closeness = 1.0 / (dist + 1e-5);
//            reward_hunter += closeness * 15;
//            reward_victim -= closeness * 5;
//
//            // Вместо: reward_victim += 25.0;
//            double edge_penalty = 0.0;
//            if (state.runner_x < 100 || state.runner_x > 700 ||
//                state.runner_y < 100 || state.runner_y > 500)
//            {
//                edge_penalty = -5.0;  // штраф за попытку "спрятаться в углу"
//            }
//            reward_victim += 10.0 + edge_penalty;
//
//            if (runner_sector == chaser_sector) {
//                reward_hunter += 3.0;  // не сильно
//                reward_victim -= 1.5;
//            }
//            else{
//                reward_hunter -= 1.5;  // не сильно
//                reward_victim += 2.0;
//            }
//
//            double runner_movement = std::sqrt(dx_v*dx_v + dy_v*dy_v);
//            if (runner_movement < 0.1)
//                reward_victim -= 2.0;
//
//            if (dist < 20.0) {
//                reward_hunter += 20.0;
//                reward_victim -= 20.0;
//                break;
//            }
//
//            if (i >= step - 1) {
//                reward_victim += 25.0;
//                reward_hunter -= 15.0;
//            }
//
//        }
//
//
//
////        bool train_victim = (epoch < 500 || (epoch >= 1000 && epoch < 1500)) || (epoch >= 2000);
////        bool train_hunter = ((epoch >= 500 && epoch < 1000) || (epoch >= 1500 && epoch < 2000)) || (epoch >= 2000);
//
//        // === ЦИЯ ===
////        for (int i = 0; i < hunter_net.layer_0.size(); ++i) {
////            if (i == hunter_sector)
////                hunter_net.layer_0[i].giveReward(reward_hunter);
////            else
////                hunter_net.layer_0[i].giveReward(0.0); // или маленький штраф
////            hunter_net.layer_0[i].mutate(0.2);
////        }
////
////        for (auto& n : hunter_net.layer_1_x) n.giveReward(reward_hunter), n.mutate(0.2);
////        for (auto& n : hunter_net.layer_1_y) n.giveReward(reward_hunter), n.mutate(0.2);
////
////        hunter_net.layer_2.giveReward(reward_hunter);
////        hunter_net.layer_2.mutate(0.2);
////
////
////        for (int i = 0; i < victim_net.layer_0.size(); ++i) {
////            if (i == victim_sector)
////                victim_net.layer_0[i].giveReward(reward_victim);
////            else
////                victim_net.layer_0[i].giveReward(0.0);
////            victim_net.layer_0[i].mutate(0.2);
////        }
////
////        for (auto& n : victim_net.layer_1_x) n.giveReward(reward_victim), n.mutate(0.2);
////        for (auto& n : victim_net.layer_1_y) n.giveReward(reward_victim), n.mutate(0.2);
////
////        victim_net.layer_2.giveReward(reward_victim);
////        victim_net.layer_2.mutate(0.2);
////        if (train_hunter && hunter_sector >= 0) {
////            for (int i = 0; i < hunter_net.layer_0.size(); ++i) {
////                hunter_net.layer_0[i].giveReward(i == hunter_sector ? reward_hunter : 0.0);
////                hunter_net.layer_0[i].mutate(0.2);
////            }
////
////            for (auto& n : hunter_net.layer_1_x) n.giveReward(reward_hunter), n.mutate(0.2);
////            for (auto& n : hunter_net.layer_1_y) n.giveReward(reward_hunter), n.mutate(0.2);
////
////            hunter_net.layer_2.giveReward(reward_hunter);
////            hunter_net.layer_2.mutate(0.2);
////
////        }
//        if (train_hunter && hunter_sector >= 0) {
//            for (int i = 0; i < hunter_net.layer_0.size(); ++i) {
//                double r = (i == hunter_sector) ? reward_hunter : 0.0;
//                hunter_net.layer_0[i].giveReward(r);
//                if (i == hunter_sector)
//                    hunter_net.layer_0[i].mutate(0.2);
//            }
//
//            hunter_net.layer_1_x[hunter_sector].giveReward(reward_hunter);
//            hunter_net.layer_1_x[hunter_sector].mutate(0.2);
//
//            hunter_net.layer_1_y[hunter_sector].giveReward(reward_hunter);
//            hunter_net.layer_1_y[hunter_sector].mutate(0.2);
//
//
//
//            double dist_to_target = EnvUtils::distance(state.chaser_x, state.chaser_y, hunter_move[0], hunter_move[1]);
//            double vec_reward_hunter = std::clamp((800.0 - dist_to_target) / 800.0, 0.0, 1.0);
//            hunter_net.layer_2.giveReward(vec_reward_hunter);
//            hunter_net.layer_2.mutate(0.2);
//
//        }
//
//
////        if (train_victim && victim_sector >= 0) {
////            for (int i = 0; i < victim_net.layer_0.size(); ++i) {
////                victim_net.layer_0[i].giveReward(i == victim_sector ? reward_victim : 0.0);
////                victim_net.layer_0[i].mutate(0.2);
////            }
////
////            for (auto& n : victim_net.layer_1_x) n.giveReward(reward_victim), n.mutate(0.2);
////            for (auto& n : victim_net.layer_1_y) n.giveReward(reward_victim), n.mutate(0.2);
////            victim_net.layer_2.giveReward(reward_victim);
////            victim_net.layer_2.mutate(0.2);
////
////        }
//        if (train_victim && victim_sector >= 0) {
//            for (int i = 0; i < victim_net.layer_0.size(); ++i) {
//                double r = (i == victim_sector) ? reward_victim : 0.0;
//                victim_net.layer_0[i].giveReward(r);
//                if (i == victim_sector)
//                    victim_net.layer_0[i].mutate(0.2);
//            }
//
//            victim_net.layer_1_x[victim_sector].giveReward(reward_victim);
//            victim_net.layer_1_x[victim_sector].mutate(0.2);
//
//            victim_net.layer_1_y[victim_sector].giveReward(reward_victim);
//            victim_net.layer_1_y[victim_sector].mutate(0.2);
//
//            double dist_from_chaser = EnvUtils::distance(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);
//            double vec_reward_victim = std::clamp(dist_from_chaser / 800.0, 0.0, 1.0);
//            victim_net.layer_2.giveReward(vec_reward_victim);
//            victim_net.layer_2.mutate(0.2);
//
//        }
//
//
//        if (reward_hunter > best_reward_hunter) {
//            best_reward_hunter = reward_hunter;
//            best_hunter = hunter_net;
//            best_hunter.setSectorPoints(&sector_points);
//
//
//            frozen_hunter = best_hunter;
//            frozen_hunter.setSectorPoints(&sector_points);
//        }
//
//        if (reward_victim > best_reward_victim) {
//            best_reward_victim = reward_victim;
//            best_victim = victim_net;
//            best_victim.setSectorPoints(&sector_points);
//
//
//            frozen_victim = best_victim;
//            frozen_victim.setSectorPoints(&sector_points);
//        }
//
//        if (epoch % 200 == 0)
//            std::cout << "Epoch: " << epoch << ", Hunter reward: " << reward_hunter
//                      << ", Victim reward: " << reward_victim << std::endl;
//
//    }
//
//    best_hunter.saveToFile("hunter_weights_with_critic.txt");
//    best_victim.saveToFile("victim_weights_with_critic.txt");
//
//    return 0;

int main() {
    while (true) {
        sf::RenderWindow window(sf::VideoMode(800, 600), "Hide and Seek - Toggle FX");
        window.setFramerateLimit(60);

        sf::Font font;
        if (!font.loadFromFile("BrownieStencil-8O8MJ.ttf")) {
            std::cerr << "Ошибка загрузки шрифта\n";
            return -1;
        }

        sf::Texture fireTexture, heartTexture;
        if (!fireTexture.loadFromFile("fire1_64.png") ||
            !heartTexture.loadFromFile("hearty_strip6.png")) {
            std::cerr << "Ошибка загрузки анимаций\n";
            return -1;
        }

        sf::Texture hunterTexture, puppetTexture;
        if (!hunterTexture.loadFromFile("HunterOrc.png") ||
            !puppetTexture.loadFromFile("priest-NESW.png")) {
            std::cerr << "Ошибка загрузки персонажей\n";
            return -1;
        }

        // Спрайты и параметры анимации
        sf::Sprite orcSprite(hunterTexture);
        const int orcW = 32, orcH = 32, orcFrames = 10, orcRows = 5;
        int orcFrame = 0, orcRow = 0;
        orcSprite.setScale(8.f, 8.f);
        orcSprite.setPosition(0, 180);

        sf::Sprite puppetSprite(puppetTexture);
        const int puppetW = 48, puppetH = 64, puppetFrames = 3, puppetRow = 2;
        int puppetFrame = 0;
        puppetSprite.setScale(5.f, 5.f);
        puppetSprite.setPosition(560, 120);

        const int fxW = 64, fxH = 64, fxFrames = 6;
        int fxFrame = 0;
        sf::Sprite fxSprite;
        fxSprite.setOrigin(fxW / 2.f, fxH / 2.f);
        fxSprite.setScale(0.7f, 0.7f);

        sf::RectangleShape toggleBar(sf::Vector2f(80, 30));
        toggleBar.setPosition(360, 400);
        toggleBar.setOutlineThickness(2);
        toggleBar.setOutlineColor(sf::Color::White);

        bool isOn = true;
        sf::Vector2f knobPosOn = toggleBar.getPosition() + sf::Vector2f(50, 0);
        sf::Vector2f knobPosOff = toggleBar.getPosition();

        sf::Text label;
        label.setFont(font);
        label.setString("Choose your puppet");
        label.setCharacterSize(20);
        label.setFillColor(sf::Color::Red);
        label.setPosition(toggleBar.getPosition().x - 40, toggleBar.getPosition().y + 50);

        sf::Text exitText;
        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(24);
        exitText.setFillColor(sf::Color::Red);
        exitText.setPosition(740, 10);

        std::string menuItems[] = {"Start Game", "Continue Game"};
        const int MENU_ITEMS = 2;
        sf::Text menu[MENU_ITEMS];
        for (int i = 0; i < MENU_ITEMS; ++i) {
            menu[i].setFont(font);
            menu[i].setString(menuItems[i]);
            menu[i].setCharacterSize(48);
            menu[i].setStyle(sf::Text::Bold);
            menu[i].setPosition(250, 200 + i * 70);
        }

        sf::Clock clock;
        float animTimer = 0.0f, animSpeed = 0.12f;
        sf::Color bgColor = sf::Color::White;
        sf::Color targetBgColor = bgColor;

        sf::RectangleShape overlay(sf::Vector2f(0, 600));
        overlay.setFillColor(bgColor);
        bool animating = false;
        float overlaySpeed = 20.f;
        bool overlayLTR = true;

        bool launchGame = false;
        bool asPuppet = true;
        bool exitApp = false;

        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            animTimer += dt;

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    exitApp = true;
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                    if (toggleBar.getGlobalBounds().contains(mouse)) {
                        isOn = !isOn;
                        animating = true;
                        overlay.setSize({0, 600});
                        targetBgColor = isOn ? sf::Color::White : sf::Color::Black;
                        overlay.setFillColor(targetBgColor);
                        overlayLTR = isOn;
                    }

                    if (exitText.getGlobalBounds().contains(mouse)) {
                        window.close();
                        exitApp = true;
                    }

                    if (menu[0].getGlobalBounds().contains(mouse)) {
                        asPuppet = isOn;
                        launchGame = true;
                        window.close();
                    }
                }
            }

            if (animTimer >= animSpeed) {
                fxFrame = (fxFrame + 1) % fxFrames;
                if (!isOn) {
                    fxSprite.setScale(1.1f, 1.1f);
                    orcFrame = (orcFrame + 1) % orcFrames;
                    if (orcFrame == 0) orcRow = (orcRow + 1) % orcRows;
                } else {
                    puppetFrame = (puppetFrame + 1) % puppetFrames;
                    fxSprite.setScale(0.7f, 0.7f);
                }
                animTimer = 0.0f;
            }

            orcSprite.setTextureRect({orcFrame * orcW, orcRow * orcH, orcW, orcH});
            puppetSprite.setTextureRect({puppetFrame * puppetW, puppetRow * puppetH, puppetW, puppetH});
            fxSprite.setTexture(isOn ? heartTexture : fireTexture);
            fxSprite.setTextureRect({fxFrame * fxW, 0, fxW, fxH});
            sf::Vector2f offset = isOn ? sf::Vector2f(30.f, 12.f) : sf::Vector2f(0.f, 0.f);
            fxSprite.setPosition((isOn ? knobPosOn : knobPosOff) + offset);
            toggleBar.setFillColor(isOn ? sf::Color(200, 200, 200) : sf::Color(60, 60, 60));

            if (animating) {
                float newWidth = overlay.getSize().x + overlaySpeed;
                if (newWidth >= 800) {
                    newWidth = 800;
                    animating = false;
                    bgColor = targetBgColor;
                }
                overlay.setSize({newWidth, 600});
                overlay.setPosition(overlayLTR ? 0 : 800 - newWidth, 0);
            }

            window.clear(bgColor);

            for (int i = 0; i < MENU_ITEMS; ++i) {
                sf::Text shadow = menu[i];
                shadow.setFillColor(sf::Color(0, 0, 0, 150));
                shadow.move(4, 4);
                menu[i].setFillColor(i == 0 ? sf::Color(255, 50, 50) : sf::Color(230, 230, 230));
                window.draw(shadow);
                window.draw(menu[i]);
            }

            window.draw(isOn ? puppetSprite : orcSprite);
            window.draw(toggleBar);
            window.draw(fxSprite);
            window.draw(label);
            window.draw(exitText);
            if (animating) window.draw(overlay);
            window.display();
        }

        if (exitApp) break;
        int humanFire = 0;
        if (launchGame) {
            bool continuePlaying = showCharacterWindow(
                    asPuppet ? puppetTexture : hunterTexture,
                    asPuppet ? "Human" : "Orc",
                    asPuppet,humanFire
            );

            while (true) {
                bool keepPlaying = showCharacterWindow(
                        asPuppet ? puppetTexture : hunterTexture,
                        asPuppet ? "Human" : "Orc",
                        asPuppet, humanFire
                );

                if (!keepPlaying) break;
            }

        }
    }

    return 0;
}

