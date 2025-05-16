#pragma once
#include <SFML/Graphics.hpp>
#include "Character.h"
using namespace sf;
using namespace std;

class Projectile {
public:
    Projectile(float xPos, float yPos, float direction, float speed, Texture& texture)
        : x(xPos), y(yPos), initialX(xPos), velocityX(direction * speed), speed(speed), active(true) {
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);
        sprite.setPosition(x, y);
        sprite.setOrigin(texture.getSize().x / 2.0f, texture.getSize().y / 2.0f);
    }

    void update(float deltaTime) {
        if (!active) return;
        x += velocityX * deltaTime;
        sprite.setPosition(x, y);
        // Deactivate if the projectile has traveled 10 units
        if (abs(x - initialX) >= 640.0f) {
            active = false;
        }
    }

    void draw(RenderWindow& window) {
        if (active) {
            window.draw(sprite);
        }
    }

    bool checkCollision(Character& character) {
        if (!active) return false;
        if (sprite.getGlobalBounds().intersects(character.characterSprite.getGlobalBounds())) {
            if (!character.isInvincible()) {
                character.decreaseHP(1);
                float pushback = (character.x < x) ? -10.0f : 10.0f;
                character.applyPushback(pushback);
            }
            active = false;
            return true;
        }
        return false;
    }

    bool isActive() const { return active; }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }

private:
    float x, y;
    float initialX; // Store initial x position to track distance
    float velocityX;
    float speed;
    bool active;
    Sprite sprite;
};