#pragma once
#include "Enemy.h"
#include <SFML/Graphics.hpp>
using namespace sf;
using namespace std;

class Boss : public Enemy {
private:
    Texture& attackTexture;
    Texture& projectileTexture;
    Animation attackAnim;
    Sprite projectiles[5];
    bool projectileActive[5];
    float projectileStartX[5];
    float attackTimer;
    bool isAttacking;
    float speed;
    const int cell_size = 64;
    static const int MAX_PROJECTILES = 5;
    float patrolCenterX;
    static const int PATROL_RANGE = 320;
    static const int PROJECTILE_RANGE = 640;
    bool facingRight;

public:
    Boss(Texture& idleTexture, Texture& attackTexture, Texture& projectileTexture, float x, float y, int hp, int attackSpriteCount)
        : Enemy(idleTexture, attackTexture, x, y, hp, attackSpriteCount),
          attackTexture(attackTexture), projectileTexture(projectileTexture), attackTimer(0.0f), isAttacking(false), speed(100.0f), patrolCenterX(x), facingRight(true) {
        sprite.setScale(2.0f, 2.0f);
        Ewidth = sprite.getGlobalBounds().width;
        Eheight = sprite.getGlobalBounds().height;
        sprite.setOrigin(Ewidth / 2.0f, 0.0f);
        Ewidth = sprite.getGlobalBounds().width;

        // Init attack animation
        int frameWidth = attackTexture.getSize().x / attackSpriteCount;
        int frameHeight = attackTexture.getSize().y;
        for (int i = 0; i < attackSpriteCount; ++i) {
            attackAnim.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }

        // Init projectiles
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
            projectileActive[i] = false;
            projectileStartX[i] = 0.0f;
        }
    }

    void update(float deltaTime) override {
        if (!isAlive()) return;
        attackTimer += deltaTime;
        bool isMoving = true;

        // Handle attack
        if (attackTimer >= 2.0f) {
            isAttacking = true;
            attackAnim.update(deltaTime);
            sprite.setTexture(attackTexture);
            sprite.setTextureRect(attackAnim.getCurrentFrame());

            // Spawn projectile
            for (int i = 0; i < MAX_PROJECTILES; ++i) {
                if (!projectileActive[i]) {
                    projectiles[i].setTexture(projectileTexture);
                    float projX = facingRight ? x + Ewidth / 2.0f : x - Ewidth / 2.0f;
                    projectiles[i].setPosition(projX, y + Eheight / 2);
                    projectileActive[i] = true;
                    projectileStartX[i] = projX;
                    projectiles[i].setScale(facingRight ? 1.0f : -1.0f, 1.0f);
                    break;
                }
            }
            attackTimer = 0.0f;
        } else if (attackTimer >= 0.5f && isAttacking) {
            isAttacking = false;
            updateAnimation(deltaTime, isMoving);
        } else {
            updateAnimation(deltaTime, isMoving);
        }

        // Patrol movement
        x += speed * deltaTime;
        bool previousFacingRight = facingRight;
        if (x <= patrolCenterX - PATROL_RANGE || x >= patrolCenterX + PATROL_RANGE) {
            speed = -speed;
            facingRight = speed > 0;
        } else {
            facingRight = speed > 0;
        }
        if (facingRight != previousFacingRight) {
            sprite.setScale(facingRight ? 2.0f : -2.0f, 2.0f);
        }
        sprite.setPosition(x, y);

        // Update projectiles
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
            if (projectileActive[i]) {
                projectiles[i].move(-200.0f * deltaTime, 0);
                if (abs(projectiles[i].getPosition().x - projectileStartX[i]) >= PROJECTILE_RANGE) {
                    projectileActive[i] = false;
                }
            }
        }
    }

    void draw(RenderWindow& window) override {
        if (isAlive()) {
            window.draw(sprite);
            for (int i = 0; i < MAX_PROJECTILES; ++i) {
                if (projectileActive[i]) {
                    window.draw(projectiles[i]);
                }
            }
        }
    }

    void checkCollision(Character& character) override {
        if (!isAlive() || character.isInvincible()) return;

        // Boss body collision
        FloatRect playerBounds(character.x, character.y, character.Pwidth, character.Pheight);
        FloatRect enemyBounds(x - Ewidth / 2.0f, y, Ewidth, Eheight);
        if (playerBounds.intersects(enemyBounds)) {
            if (character.isJumping) {
                hp -= 1;
            }
        }

        // Projectile collision
        for (int i = 0; i < MAX_PROJECTILES; ++i) {
            if (projectileActive[i]) {
                FloatRect projBounds = projectiles[i].getGlobalBounds();
                if (playerBounds.intersects(projBounds) && !character.isInvincible()) {
                    character.decreaseHP(1);
                    projectileActive[i] = false;
                }
            }
        }
    }

    FloatRect getBounds() const override {
        return FloatRect(x - Ewidth / 2.0f, y, Ewidth, Eheight);
    }
};