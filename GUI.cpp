//
// Created by 2005s on 18.05.2025.
//

#include "GUI.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <array>
#include "NeuroCore.h"
#include "SimulationUtils.h"
void saveProgressToFile(const std::string& filename, int resourceCount, const int stats[3]) {
    std::ofstream file(filename);
    if (!file) return;
    file << "resourceCount " << resourceCount << "\n";
    file << "Attentiveness " << stats[0] << "\n";
    file << "Intelligence " << stats[1] << "\n";
    file << "Agility " << stats[2] << "\n";
}

bool loadProgressFromFile(const std::string& filename, int& resourceCount, int stats[3]) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string key;
    int value;
    int count = 0;
    while (file >> key >> value) {
        if (key == "resourceCount") resourceCount = value;
        else if (key == "Attentiveness") stats[0] = value;
        else if (key == "Intelligence")  stats[1] = value;
        else if (key == "Agility")       stats[2] = value;
        ++count;
    }

    return count >= 4;
}

bool showCharacterWindow(sf::Texture& characterTexture, const std::string& characterName, bool isPuppet, int& resourceCount) {
    sf::RenderWindow charWindow(sf::VideoMode(800, 600), "Character Preview - " + characterName);
    charWindow.setFramerateLimit(60);

    sf::Sprite characterSprite(characterTexture);
    int frameW = isPuppet ? 48 : 32;
    int frameH = isPuppet ? 64 : 32;
    characterSprite.setTextureRect({0, isPuppet ? 2 * frameH : 0, frameW, frameH});
    characterSprite.setScale(isPuppet ? 6.5f : 10.f, isPuppet ? 6.5f : 10.f);
    characterSprite.setPosition(isPuppet ? 450.f : 500.f, isPuppet ? 20.f : 100.f);

    sf::Color bgColor = isPuppet ? sf::Color::White : sf::Color(30, 30, 30);
    sf::Color textColor = isPuppet ? sf::Color::Black : sf::Color(230, 230, 230);
    sf::Color btnColor = isPuppet ? sf::Color(100, 200, 100) : sf::Color(80, 160, 80);
    sf::Color backBtnColor = isPuppet ? sf::Color(200, 100, 100) : sf::Color(140, 60, 60);

    sf::Font font;
    if (!font.loadFromFile("BrownieStencil-8O8MJ.ttf")) {
        std::cerr << "Ошибка загрузки шрифта\n";
        return false;
    }



    sf::Text title(characterName, font, 36);
    title.setFillColor(sf::Color::Red);
    sf::FloatRect textRect = title.getLocalBounds();
    title.setOrigin(textRect.width / 2.f, textRect.height / 2.f);
    title.setPosition(800 / 2.f, 30.f);

    int statsValues[3] = {1, 1, 1};
    resourceCount = 0;
    std::string saveFile = isPuppet ? "Human.txt" : "Orc.txt";
    loadProgressFromFile(saveFile, resourceCount, statsValues);

    std::string statNames[] = {"Attentiveness", "Intelligence", "Agility"};
    sf::RectangleShape progressBars[3];
    sf::RectangleShape plusButtons[3];
    sf::Text plusTexts[3];
    sf::Text levelTexts[3];

    for (int i = 0; i < 3; ++i) {
        progressBars[i].setSize({160, 20});
        progressBars[i].setPosition(50, 120 + i * 50);
        progressBars[i].setFillColor(sf::Color(100, 100, 100));

        plusButtons[i].setSize({30, 30});
        plusButtons[i].setPosition(progressBars[i].getPosition().x + 180, progressBars[i].getPosition().y - 5);
        plusButtons[i].setFillColor(sf::Color(180, 80, 80));

        plusTexts[i].setFont(font);
        plusTexts[i].setCharacterSize(24);
        plusTexts[i].setString("+");
        plusTexts[i].setFillColor(sf::Color::White);
        plusTexts[i].setPosition(plusButtons[i].getPosition().x + 8, plusButtons[i].getPosition().y - 2);

        levelTexts[i].setFont(font);
        levelTexts[i].setCharacterSize(18);
        levelTexts[i].setFillColor(isPuppet ? sf::Color::White : sf::Color::Black);
        levelTexts[i].setString(std::to_string(statsValues[i]));
        levelTexts[i].setPosition(progressBars[i].getPosition().x + 70, progressBars[i].getPosition().y - 2);
    }

    sf::Text statLabels[3];
    for (int i = 0; i < 3; ++i) {
        statLabels[i].setFont(font);
        statLabels[i].setCharacterSize(18);
        statLabels[i].setFillColor(textColor);
        statLabels[i].setString(statNames[i]);
        statLabels[i].setPosition(50, 100 + i * 50);
    }

    sf::RectangleShape startButton(sf::Vector2f(150, 40));
    startButton.setFillColor(btnColor);
    startButton.setPosition(50, 300);
    sf::Text startText("Start Game", font, 20);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(startButton.getPosition().x + 15, startButton.getPosition().y + 5);

    sf::RectangleShape backButton(sf::Vector2f(150, 40));
    backButton.setFillColor(backBtnColor);
    backButton.setPosition(50, 360);
    sf::Text backText("Go Back", font, 20);
    backText.setFillColor(sf::Color::White);
    backText.setPosition(backButton.getPosition().x + 25, backButton.getPosition().y + 5);

    int frame = 0;
    int maxFrames = isPuppet ? 3 : 10;
    int row = isPuppet ? 2 : 0;
    float animTimer = 0.f;
    float animSpeed = 0.12f;
    sf::Clock clock;



    std::string description = isPuppet ? "Note:\nYou play as a victim.\n Your main task is to do everything to survive)"
                                       : "Note:\nYou play as a hunter.\n your main task is to catch your victim at any cost)";

    sf::RectangleShape descBox(sf::Vector2f(isPuppet ? 400 : 430, 90));
    descBox.setFillColor(sf::Color(255, 255, 100, 80));
    descBox.setPosition(20, 500);

    sf::Text descText(description, font, 16);
    descText.setFillColor(textColor);
    descText.setPosition(descBox.getPosition().x + 10, descBox.getPosition().y + 8);

    sf::Texture fireTexture, heartTexture;
    fireTexture.loadFromFile("fire1_64.png");
    heartTexture.loadFromFile("hearty_strip6.png");

    const int fxW = 64, fxH = 64, fxFrames = 6;
    int fxFrame = 0;

    sf::Sprite fxSprite;
    fxSprite.setTexture(isPuppet ? fireTexture : heartTexture);
    fxSprite.setOrigin(fxW / 2.f, fxH / 2.f);
    fxSprite.setScale(0.6f, 0.6f);
    fxSprite.setPosition(700.f, 20.f);

    sf::Text resourceText;
    resourceText.setFont(font);
    resourceText.setCharacterSize(24);
    resourceText.setFillColor(isPuppet ? sf::Color::Black : sf::Color::White);
    resourceText.setString("x" + std::to_string(resourceCount));
    resourceText.setPosition(fxSprite.getPosition().x + 30.f, fxSprite.getPosition().y - 10.f);
    int totalStatPoints = statsValues[0] + statsValues[1] + statsValues[2];

    while (charWindow.isOpen()) {
        float dt = clock.restart().asSeconds();
        animTimer += dt;

        sf::Event event;
        while (charWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return false;

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mouse = charWindow.mapPixelToCoords(sf::Mouse::getPosition(charWindow));
                if (startButton.getGlobalBounds().contains(mouse)) {
                    charWindow.close();

                    int puppetStats[3] = {1,1,1}, orcStats[3] = {1,1,1};
                    int dummy = 0;
                    loadProgressFromFile("Human.txt", dummy, puppetStats);
                    loadProgressFromFile("Orc.txt", dummy, orcStats);

                    auto hasLevel2 = [](const int stats[3]) {
                        return stats[0] == 2 || stats[1] == 2 || stats[2] == 2;
                    };
                    auto hasLevel3 = [](const int stats[3]) {
                        return stats[0] > 2 || stats[1] > 2 || stats[2] > 2;
                    };

                    bool isLevel2 = hasLevel2(puppetStats) || hasLevel2(orcStats);
                    bool isLevel3 = hasLevel3(puppetStats) || hasLevel3(orcStats);

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


                    if (!isLevel2 && !isLevel3) {
                        if (isPuppet) {
                            hunterLevel = HunterDifficulty::EASY;
                            victim_net.loadFromFile("victim_trained_against_script.txt");
                            runMatchWithGUI(AgentControl::SCRIPTED_HUNTER, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
                        } else {
                            victimLevel = VictimDifficulty::EASY;
                            hunter_net.loadFromFile("hunter_trained_against_script.txt");
                            runMatchWithGUI(AgentControl::SCRIPTED_VICTIM, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
                        }
                    }

                    else if (isLevel2 && !isLevel3) {
                        hunterLevel = HunterDifficulty::MEDIUM;
                        victimLevel = VictimDifficulty::MEDIUM;
                        runMatchWithGUI(AgentControl::SCRIPTED_HUNTER, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
                    }

                    else if (isLevel3) {
                        hunterLevel = HunterDifficulty::HARD; //No time to write Net_Learn for dif levels as i want all the method in ugliest code ever Trainer;((
                        victimLevel = VictimDifficulty::HARD;
                        runMatchWithGUI(AgentControl::SCRIPTED_HUNTER, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
                    }


                    sf::RenderWindow resultWindow(sf::VideoMode(400, 200), "Result");
                    sf::Text resultText(isPuppet ? "Hunter Win!" : "Victim Win!", font, 36);
                    resultText.setFillColor(isPuppet ? sf::Color::Red : sf::Color::Blue);
                    resultText.setPosition(100, 80);

                    while (resultWindow.isOpen()) {
                        sf::Event event;
                        while (resultWindow.pollEvent(event)) {
                            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed)
                                resultWindow.close();
                        }
                        resultWindow.clear(sf::Color::Black);
                        resultWindow.draw(resultText);
                        resultWindow.display();
                    }

                    resourceCount++;
                    resourceText.setString("x" + std::to_string(resourceCount));
                    saveProgressToFile(saveFile, resourceCount, statsValues);



//                    if (isPuppet) {
//
//                        if (totalStatPoints <= 3)
//                            hunterLevel = HunterDifficulty::EASY;
//                        else if (totalStatPoints <= 6)
//                            hunterLevel = HunterDifficulty::MEDIUM;
//                        else
//                            hunterLevel = HunterDifficulty::HARD;
//                        EnvironmentGUI gui;
//                        Net_Hunter hunter_net(4);
//                        Net_Victim victim_net(4);
//
//                        std::map<int, std::vector<std::pair<double, double>>> sector_points;
//                        std::array<std::pair<double, double>, 4> sector_centers = {
//                                std::make_pair(200.0, 150.0),
//                                std::make_pair(600.0, 150.0),
//                                std::make_pair(200.0, 450.0),
//                                std::make_pair(600.0, 450.0)
//                        };
//
//                        for (int sector = 0; sector < 4; ++sector) {
//                            std::vector<std::pair<double, double>> points;
//                            double base_x = (sector % 2 == 0) ? 0 : 400;
//                            double base_y = (sector / 2 == 0) ? 0 : 300;
//                            for (int i = 1; i <= 5; ++i)
//                                for (int j = 1; j <= 4; ++j)
//                                    points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
//                            sector_points[sector] = points;
//                        }
//
//                        hunter_net.setSectorPoints(&sector_points);
//                        victim_net.setSectorPoints(&sector_points);
//                        gui.setSectorPoints(&sector_points);
//
//                        victim_net.loadFromFile("victim_trained_against_script.txt");
//
//                        runMatchWithGUI(AgentControl::SCRIPTED_HUNTER, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
//
//
//                        sf::RenderWindow resultWindow(sf::VideoMode(400, 200), "Result");
//                        sf::Text resultText("Hunter Win!", font, 36);
//                        resultText.setFillColor(sf::Color::Red);
//                        resultText.setPosition(100, 80);
//
//                        while (resultWindow.isOpen()) {
//                            sf::Event event;
//                            while (resultWindow.pollEvent(event)) {
//                                if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
//                                    resultWindow.close();
//                                }
//                            }
//                            resultWindow.clear(sf::Color::Black);
//                            resultWindow.draw(resultText);
//                            resultWindow.display();
//                        }
//                        resourceCount++;
//                        resourceText.setString("x" + std::to_string(resourceCount));
//                        saveProgressToFile(saveFile, resourceCount, statsValues);
//
//                    }else {
//                        // Orc
//                        if (totalStatPoints <= 3)
//                            victimLevel = VictimDifficulty::EASY;
//                        else if (totalStatPoints <= 6)
//                            victimLevel = VictimDifficulty::MEDIUM;
//                        else
//                            victimLevel = VictimDifficulty::HARD;
//                        EnvironmentGUI gui;
//                        Net_Hunter hunter_net(4);
//                        Net_Victim victim_net(4);
//
//                        std::map<int, std::vector<std::pair<double, double>>> sector_points;
//                        std::array<std::pair<double, double>, 4> sector_centers = {
//                                std::make_pair(200.0, 150.0),
//                                std::make_pair(600.0, 150.0),
//                                std::make_pair(200.0, 450.0),
//                                std::make_pair(600.0, 450.0)
//                        };
//
//                        for (int sector = 0; sector < 4; ++sector) {
//                            std::vector<std::pair<double, double>> points;
//                            double base_x = (sector % 2 == 0) ? 0 : 400;
//                            double base_y = (sector / 2 == 0) ? 0 : 300;
//                            for (int i = 1; i <= 5; ++i)
//                                for (int j = 1; j <= 4; ++j)
//                                    points.emplace_back(base_x + i * 400.0 / 6.0, base_y + j * 300.0 / 5.0);
//                            sector_points[sector] = points;
//                        }
//
//                        hunter_net.setSectorPoints(&sector_points);
//                        victim_net.setSectorPoints(&sector_points);
//                        gui.setSectorPoints(&sector_points);
//
//                        hunter_net.loadFromFile("hunter_trained_against_script.txt");
//
//                        runMatchWithGUI(AgentControl::SCRIPTED_VICTIM, hunter_net, victim_net, sector_points, sector_centers, gui, statsValues);
//
//
//                        sf::RenderWindow resultWindow(sf::VideoMode(400, 200), "Result");
//                        sf::Text resultText("Victim Win!", font, 36);
//                        resultText.setFillColor(sf::Color::Blue);
//                        resultText.setPosition(100, 80);
//
//                        while (resultWindow.isOpen()) {
//                            sf::Event event;
//                            while (resultWindow.pollEvent(event)) {
//                                if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed) {
//                                    resultWindow.close();
//                                }
//                            }
//                            resultWindow.clear(sf::Color::Black);
//                            resultWindow.draw(resultText);
//                            resultWindow.display();
//                        }
//
//                        resourceCount++;
//                        resourceText.setString("x" + std::to_string(resourceCount));
//                        saveProgressToFile(saveFile, resourceCount, statsValues);
//                    }

                    return true;
                }

                for (int i = 0; i < 3; ++i) {
                    if (plusButtons[i].getGlobalBounds().contains(mouse)) {
                        if (statsValues[i] < 3 && resourceCount > 0) {
                            statsValues[i]++;
                            resourceCount--;
                            levelTexts[i].setString(std::to_string(statsValues[i]));
                            resourceText.setString("x" + std::to_string(resourceCount));
                            saveProgressToFile(saveFile, resourceCount, statsValues);

                        }
                    }
                }
                if (backButton.getGlobalBounds().contains(mouse)) {
                    charWindow.close(); return false;
                }
            }
        }

        if (animTimer >= animSpeed) {
            frame = (frame + 1) % maxFrames;
            characterSprite.setTextureRect({frame * frameW, row * frameH, frameW, frameH});
            fxFrame = (fxFrame + 1) % fxFrames;
            fxSprite.setTextureRect({fxFrame * fxW, 0, fxW, fxH});
            animTimer = 0.f;
        }

        charWindow.clear(bgColor);
        charWindow.draw(title);
        charWindow.draw(characterSprite);

        for (int i = 0; i < 3; ++i) {
            charWindow.draw(statLabels[i]);
            charWindow.draw(progressBars[i]);
            sf::RectangleShape filled(sf::Vector2f(40.f * statsValues[i], 20));
            filled.setPosition(progressBars[i].getPosition());
            filled.setFillColor(isPuppet ? sf::Color::Green : sf::Color::Red);
            charWindow.draw(filled);
            charWindow.draw(plusButtons[i]);
            charWindow.draw(plusTexts[i]);
            charWindow.draw(levelTexts[i]);
        }

        charWindow.draw(fxSprite);
        charWindow.draw(resourceText);
        charWindow.draw(startButton);
        charWindow.draw(startText);
        charWindow.draw(backButton);
        charWindow.draw(backText);
        charWindow.draw(descBox);
        charWindow.draw(descText);
        charWindow.display();
    }

    return false;
}
