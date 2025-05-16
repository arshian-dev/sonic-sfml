#pragma once

#include "Enemy.h"
#include "Projectile.h"
#include "Level.h"
#include "Sonic.h"
using namespace sf;
using namespace std;

class Bee : public Enemy {
public:
    static const int MAX_PROJECTILES = 5;

    Bee(Texture& texIdle, Texture& texRun, Texture& texProjectile, float xPos, float yPos, int runSprite, Level& lvl)
        : Enemy(texIdle, texRun, xPos, yPos, 5, runSprite),
          level(lvl), baseY(yPos), movedDistance(0.0f), movingRight(true), shootTimer(0.0f), projectileCount(0) {
        projectileTexture = texProjectile;
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
            projectiles[i] = nullptr;
        }
    }

    ~Bee() {
        for (int i = 0; i < projectileCount; ++i) {
            delete projectiles[i];
        }
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

        y = baseY + sin(movedDistance * 0.05f) * 50.0f;
        float groundY = level.getGroundLevel() * 64.0f;
        if (y > groundY - 64.0f) {
            y = groundY - 64.0f;
        }

        setPosition(x, y);
        updateAnimation(deltaTime, isMoving);

        shootTimer += deltaTime;
        if (shootTimer >= shootInterval && projectileCount < MAX_PROJECTILES) {
            shoot();
            shootTimer = 0.0f;
        }

        for (int i = 0; i < projectileCount; ++i) {
            if (projectiles[i]) {
                projectiles[i]->update(deltaTime);
            }
        }
    }

    void draw(RenderWindow& window) override {
        if (isAlive()) {
            window.draw(sprite);
        }
        for (int i = 0; i < projectileCount; ++i) {
            if (projectiles[i]) {
                projectiles[i]->draw(window);
            }
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
                } else if (!character.isInvincible()) {
                    character.decreaseHP(1);
                    float pushback = (character.x < x) ? -10.0f : 10.0f;
                    character.applyPushback(pushback);
                }
            }
        }

        for (int i = 0; i < projectileCount; ++i) {
            if (projectiles[i] && projectiles[i]->checkCollision(character)) {
                delete projectiles[i];
                projectiles[i] = nullptr;
                for (int j = i; j < projectileCount - 1; ++j) {
                    projectiles[j] = projectiles[j + 1];
                }
                projectiles[projectileCount - 1] = nullptr;
                --projectileCount;
                --i;
            }
        }
    }

    FloatRect getBounds() const override {
        return sprite.getGlobalBounds();
    }

private:
    static constexpr float maxDistance = 300.0f;
    static constexpr float speed = 120.0f;
    static constexpr float shootInterval = 5.0f;
    Level& level;
    float baseY;
    float movedDistance;
    bool movingRight;
    float shootTimer;
    Texture projectileTexture;
    Projectile* projectiles[MAX_PROJECTILES];
    int projectileCount;

    void shoot() {
        float direction = movingRight ? 1.0f : -1.0f;
        projectiles[projectileCount++] = new Projectile(x, y, direction, 200.0f, projectileTexture);
    }
};
