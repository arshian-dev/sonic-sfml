#pragma once
#include "Sonic.h"
#include "Enemy.h"
using namespace sf;
using namespace std;

class MotoBug : public Enemy {
public:
    MotoBug(Texture& texIdle, Texture& texRun, float xPos, float yPos, int runSprite)
        : Enemy(texIdle, texRun, xPos, yPos, 2, runSprite),
          movedDistance(0.0f), movingRight(true) {
    }

    void update(float deltaTime) override {
        if (!isAlive()) return;

        bool isMoving = false;
        if (movingRight) {
            x += speed * deltaTime;
            movedDistance += speed * deltaTime;
            sprite.setScale(-1.5f, 1.5f);
            isMoving = true;
            if (movedDistance >= maxDistance) {
                movingRight = false;
                movedDistance = 0.0f;
            }
        } else {
            x -= speed * deltaTime;
            movedDistance += speed * deltaTime;
            sprite.setScale(1.5f, 1.5f);
            isMoving = true;
            if (movedDistance >= maxDistance) {
                movingRight = true;
                movedDistance = 0.0f;
            }
        }

        setPosition(x, y);
        updateAnimation(deltaTime, isMoving);
    }

    void draw(RenderWindow& window) override {
        if (isAlive()) {
            window.draw(sprite);
        }
    }

    void checkCollision(Character& character) override {
        if (!isAlive()) return;

        if (sprite.getGlobalBounds().intersects(character.characterSprite.getGlobalBounds())) {
            // Check if the character is Sonic and boosted
            Sonic* sonic = dynamic_cast<Sonic*>(&character);
            if (sonic && sonic->isBoosted) {
                hp = 0; // Instant death when Sonic is boosted
            } else {
                if (character.isJumping) {
                    hp--;
                } else if (!character.isInvincible()) {
                    character.decreaseHP(1);
                    float pushback = (character.x < x) ? -10.0f : 10.0f;
                    character.applyPushback(pushback);
                }
            }
        }
    }

    FloatRect getBounds() const override {
        return sprite.getGlobalBounds();
    }

private:
    static constexpr float maxDistance = 400.0f;
    static constexpr float speed = 150.0f;
    float movedDistance;
    bool movingRight;
};
