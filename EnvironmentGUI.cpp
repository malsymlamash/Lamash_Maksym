//
// Created by 2005s on 18.05.2025.
//
// EnvironmentGUI.cpp
#include "EnvironmentGUI.h"
#include <iostream>
#include <cmath>
#include "EnvUtils.h"

EnvironmentGUI::EnvironmentGUI(int width, int height)
        : window(sf::VideoMode(width, height), "Agent Environment") {
    window.setFramerateLimit(60);

    if (!font.loadFromFile("arial.ttf"))
        std::cerr << "Font not found!\n";

    if (!chaserTexture.loadFromFile("HunterOrc.png"))
        std::cerr << "Hunter sprite not found!\n";

    if (!runnerTexture.loadFromFile("priest-NESW.png"))
        std::cerr << "Victim sprite not found!\n";

    if (!backgroundTexture.loadFromFile("ground.png"))
        std::cerr << "Background not found!\n";

    if (!bushTexture.loadFromFile("shrub_01.png"))
        std::cerr << "Bush texture not found!\n";

    std::vector<sf::Vector2f> positions = {
            {100, 100}, {200, 400}, {600, 150}, {300, 300}, {700, 500}, {400, 100}
    };


    for (const auto& pos : positions) {
        sf::Sprite bush;
        bush.setTexture(bushTexture);
        bush.setPosition(pos);
        bush.setOrigin(bushTexture.getSize().x / 2, bushTexture.getSize().y / 2);
        bush.setScale(2.0f, 2.0f);
        bushes.push_back(bush);
        bushPositions.push_back(pos);
    }

    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setScale(
            static_cast<float>(window.getSize().x) / backgroundTexture.getSize().x,
            static_cast<float>(window.getSize().y) / backgroundTexture.getSize().y
    );

    chaserSprite.setTexture(chaserTexture);
    chaserSprite.setTextureRect({0, 0, 32, 32});
    chaserSprite.setScale(1.5f, 1.5f);

    runnerSprite.setTexture(runnerTexture);
    runnerSprite.setTextureRect({0, 2 * 64, 48, 64});
    runnerSprite.setScale(0.8f, 0.8f);
}

void EnvironmentGUI::toggleSectors() {
    showSectors = !showSectors;
}

void EnvironmentGUI::setSectorPoints(std::map<int, std::vector<std::pair<double, double>>>* ptr) {
    sector_points_ptr = ptr;
}

bool EnvironmentGUI::open() {
    return isOpen;
}

void EnvironmentGUI::drawGrassBackground() {
    window.clear();
    window.draw(backgroundSprite);
}

void EnvironmentGUI::updateBushesShake(double runner_x, double runner_y, double chaser_x, double chaser_y) {
    float time = bushShakeClock.getElapsedTime().asSeconds();
    for (size_t i = 0; i < bushes.size(); ++i) {
        float dist_r = EnvUtils::distance(runner_x, runner_y, bushPositions[i].x, bushPositions[i].y);
        float dist_c = EnvUtils::distance(chaser_x, chaser_y, bushPositions[i].x, bushPositions[i].y);

        bool near = (dist_r < 30.0 || dist_c < 30.0);
        float shake = near ? sin(time * 15.f) * 5.f : 0.0f;
        bushes[i].setRotation(shake);
    }
}

void EnvironmentGUI::update(const EnvironmentState& state) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            isOpen = false;
            window.close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab) {
            toggleSectors();
        }
    }

    // Обновление анимации жертвы
    if (std::abs(state.run_dx) > std::abs(state.run_dy))
        runnerDirection = (state.run_dx > 0) ? 1 : 3;
    else
        runnerDirection = (state.run_dy > 0) ? 2 : 0;

    runnerAnimTimer += 0.1f;
    if (runnerAnimTimer >= runnerAnimSpeed) {
        runnerFrame = (runnerFrame + 1) % 3;
        runnerAnimTimer = 0.f;
    }
    runnerSprite.setTextureRect({runnerFrame * 48, runnerDirection * 64, 48, 64});

    double dist = EnvUtils::distance(state.chaser_x, state.chaser_y, state.runner_x, state.runner_y);

    // Анимация атаки
    if (dist < 20.0) {
        runnerSprite.setOrigin(24, 32);
        chaserSprite.setPosition(state.chaser_x - 16, state.chaser_y - 16);
        chaserSprite.setTextureRect({0, 0, 32, 32});
        for (int row = 3; row <= 4; ++row) {
            int maxFrames = (row == 3) ? 8 : 4;
            for (int frame = 0; frame < maxFrames; ++frame) {
                chaserSprite.setTextureRect({frame * 32, row * 32, 32, 32});
                chaserSprite.setPosition(state.chaser_x - 16, state.chaser_y - 16);
                drawGrassBackground();
                window.draw(runnerSprite);
                window.draw(chaserSprite);
                window.display();
                sf::sleep(sf::milliseconds(60));
            }
        }
        for (int step = 0; step <= 5; ++step) {
            float angle = step * 18.f;
            float scaleY = 0.8f - step * 0.08f;
            float offsetY = step * 2.0f;
            runnerSprite.setRotation(angle);
            runnerSprite.setScale(0.8f, scaleY);
            runnerSprite.setPosition(state.runner_x, state.runner_y + offsetY);
            drawGrassBackground();
            window.draw(runnerSprite);
            window.draw(chaserSprite);
            window.display();
            sf::sleep(sf::milliseconds(80));
        }
        runnerSprite.setRotation(0);
        runnerSprite.setScale(0.8f, 0.8f);
        runnerSprite.setOrigin(0, 0);
        return;
    } else {
        useAltRunRow = !useAltRunRow;
        chaserAnimRow = useAltRunRow ? 2 : 0;
        sf::sleep(sf::milliseconds(40));
    }

    chaserAnimTimer += 0.1f;
    if (chaserAnimTimer >= chaserAnimSpeed) {
        chaserFrame = (chaserFrame + 1) % 8;
        chaserAnimTimer = 0.f;
    }
    chaserSprite.setTextureRect({chaserFrame * 32, chaserAnimRow * 32, 32, 32});
    chaserSprite.setPosition(state.chaser_x - 16, state.chaser_y - 16);

    drawGrassBackground();

    if (showSectors) {
        sf::Vertex vertical[] = {
                sf::Vertex(sf::Vector2f(400, 0), sf::Color::White),
                sf::Vertex(sf::Vector2f(400, 600), sf::Color::White)
        };
        sf::Vertex horizontal[] = {
                sf::Vertex(sf::Vector2f(0, 300), sf::Color::White),
                sf::Vertex(sf::Vector2f(800, 300), sf::Color::White)
        };
        window.draw(vertical, 2, sf::Lines);
        window.draw(horizontal, 2, sf::Lines);
        for (int i = 0; i < 4; ++i) {
            sf::Text text;
            text.setFont(font);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            text.setString(std::to_string(i));
            float x = (i % 2 == 0) ? 100.f : 500.f;
            float y = (i < 2) ? 100.f : 400.f;
            text.setPosition(x, y);
            window.draw(text);
        }
    }

    runnerSprite.setPosition(state.runner_x - 24, state.runner_y - 32);
    window.draw(runnerSprite);
    window.draw(chaserSprite);

    if (showSectors) {
        sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(state.chaser_x, state.chaser_y), sf::Color::Black),
                sf::Vertex(sf::Vector2f(state.runner_x, state.runner_y), sf::Color::Black)
        };
        window.draw(line, 2, sf::Lines);

        sf::Vertex runner_dir[] = {
                sf::Vertex(sf::Vector2f(state.runner_x, state.runner_y), sf::Color::Blue),
                sf::Vertex(sf::Vector2f(state.runner_x + state.run_dx * 10, state.runner_y + state.run_dy * 10), sf::Color::Blue)
        };
        sf::Vertex chaser_dir[] = {
                sf::Vertex(sf::Vector2f(state.chaser_x, state.chaser_y), sf::Color::Red),
                sf::Vertex(sf::Vector2f(state.chaser_x + state.chaser_dx * 10, state.chaser_y + state.chaser_dy * 10), sf::Color::Red)
        };
        window.draw(runner_dir, 2, sf::Lines);
        window.draw(chaser_dir, 2, sf::Lines);
    }

    updateBushesShake(state.runner_x, state.runner_y, state.chaser_x, state.chaser_y);

    for (auto& bush : bushes)
        window.draw(bush);


    window.display();
    sf::sleep(sf::milliseconds(10));
}

std::vector<sf::Vector2f> EnvironmentGUI::getBushPositions() {
    return bushPositions;
}




