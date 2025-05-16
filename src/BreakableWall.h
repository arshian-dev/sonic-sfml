#pragma once
#include "Obstacle.h"
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Character.h"
using namespace sf;
using namespace std;

class BreakableWall : public Obstacle {
private:
    bool isDestroyed;

public:
    BreakableWall(float posX, float posY) : Obstacle(posX, posY), isDestroyed(false) {
        if (!texture.loadFromFile("assets/breakable_wall.png")) {
            throw runtime_error("Failed to load breakable_wall.png");
        }
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
        // Static sprite setup
        int frameWidth = texture.getSize().x;
        int frameHeight = texture.getSize().y;
        animation.addFrame(IntRect(0, 0, frameWidth, frameHeight));
    }

    bool isBroken() const { return isDestroyed; }

    void destroy() { isDestroyed = true; }

    bool isNear(float charX, float charWidth, float range) const {
        if (isDestroyed) return false;
        float wallCenter = x + sprite.getGlobalBounds().width / 2;
        float charCenter = charX + charWidth / 2;
        return abs(wallCenter - charCenter) <= range;
    }

    void checkCollision(Character& character) override {
        if (isDestroyed) return;
        Obstacle::checkCollision(character);
    }

    void draw(RenderWindow& window) override {
        if (!isDestroyed) {
            Obstacle::draw(window);
        }
    }
};