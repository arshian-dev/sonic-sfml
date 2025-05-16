#pragma once

#include "Enemy.h"
#include "Character.h"
#include "Sonic.h"
using namespace sf;
using namespace std;

class BatBrain : public Enemy {
public:
    BatBrain(Texture& texIdle, Texture& texRun, float xPos, float yPos, int runSprite)
        : Enemy(texIdle, texRun, xPos, yPos, 3, runSprite),
          speed(50.0f), targetCharacter(nullptr) {
    }

    void update(float deltaTime) override {
        if (!isAlive()) return;

        bool isMoving = false;
        if (targetCharacter) {
            float targetX = targetCharacter->x;
            float targetY = targetCharacter->y;

            float dx = targetX - x;
            float dy = targetY - y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 0) {
                dx /= distance;
                dy /= distance;
                x += dx * speed * deltaTime;
                y += dy * speed * deltaTime;
                isMoving = true;
                sprite.setScale(dx > 0 ? -1.5f : 1.5f, 1.5f);
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
            Sonic* sonic = dynamic_cast<Sonic*>(&character);
            if (sonic && sonic->isBoosted) {
                hp = 0; // Instant death when Sonic is boosted
            } else {
                if (character.isJumping) {
                    hp--;
                    if (!isAlive()) {
                        x = -1000.0f;
                    }
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

    void setTarget(Character* character) {
        targetCharacter = character;
    }

private:
    float speed;
    Character* targetCharacter;
};

