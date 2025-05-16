#pragma once

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class HUD {
private:
    Font font;
    Text healthText;
    Text ringsText;
    Text scoreText; // Added for score display
    RectangleShape healthBar;

public:
    HUD() {
        if (!font.loadFromFile("assets/font.ttf")) {
            // Handle font loading error
        }

        healthText.setFont(font);
        healthText.setCharacterSize(20);
        healthText.setFillColor(Color::White);
        healthText.setPosition(10, 10);

        ringsText.setFont(font);
        ringsText.setCharacterSize(20);
        ringsText.setFillColor(Color::White);
        ringsText.setPosition(10, 40);

        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(10, 70); // Position below rings

        healthBar.setSize(Vector2f(100, 10));
        healthBar.setFillColor(Color::Green);
        healthBar.setPosition(100, 15);
    }

    void update(float deltaTime, int hp, int rings, int score) {
        healthText.setString("HP: " + to_string(hp));
        ringsText.setString("Rings: " + to_string(rings));
        scoreText.setString("Score: " + to_string(score)); // Update score display
        healthBar.setSize(Vector2f(hp * 20, 10));
    }

    void draw(RenderWindow& window) {
        window.draw(healthText);
        window.draw(ringsText);
        window.draw(scoreText); // Draw score
        window.draw(healthBar);
    }
};
