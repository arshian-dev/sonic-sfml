#pragma once
#include "Character.h"
using namespace sf;
using namespace std;

class Sonic : public Character {
public:
    Sonic(Texture& texIdle, Texture& texRun, Texture& texJump, Texture& texInvincible, Texture& texBoost, const Level& level, int runSprite, int jumpSprite, int sprintSprite)
        : Character(texIdle, texRun, texJump, texInvincible, level, runSprite, jumpSprite, sprintSprite),
          defaultMaxSpeedX(18.0f), defaultAccelRate(0.5f), isBoosted(false), boostTimer(0.0f) {
        velocityX = 2.0f; // Sonic-specific initial boost speed
        maxSpeedX = defaultMaxSpeedX;
        accelRate = defaultAccelRate;

        // Initialize boost animation
        this->texBoost = texBoost;
        int boostFrameWidth = texBoost.getSize().x / sprintSprite;
        int boostFrameHeight = texBoost.getSize().y;
        for (int i = 0; i < sprintSprite; i++) {
            boostAnim.addFrame(IntRect(i * boostFrameWidth, 0, boostFrameWidth, boostFrameHeight));
        }
    }

    void updateMovement(float deltaTime, const Level& level) override {
        // Check for space key to activate boost ability
        if (Keyboard::isKeyPressed(Keyboard::Space) && !isBoosted) {
            isBoosted = true;
            boostTimer = 2.0f; // Limit boost duration to 2 seconds
            maxSpeedX = defaultMaxSpeedX * 2.0f; // Double max speed
            accelRate = defaultAccelRate * 2.0f; // Double acceleration
        }

        // Update boost timer and deactivate boost if time is up
        if (isBoosted) {
            boostTimer -= deltaTime;
            if (boostTimer <= 0.0f) {
                isBoosted = false;
                maxSpeedX = defaultMaxSpeedX; // Revert to default max speed
                accelRate = defaultAccelRate; // Revert to default acceleration
            }
        }

        // Update animations
        if (isBoosted) {
            boostAnim.update(deltaTime);
            characterSprite.setTexture(texBoost);
            characterSprite.setTextureRect(boostAnim.getCurrentFrame());
        } else {
            // Call base class updateMovement for other animations
            Character::updateMovement(deltaTime, level);
            return;
        }

        // Handle movement (same as base class but repeated here to apply after boost animation)
        isRunning = false;

        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            characterSprite.setScale(-1.5f, 1.5f);
            isRunning = true;
            accelerationX = -accelRate;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            characterSprite.setScale(1.5f, 1.5f);
            isRunning = true;
            accelerationX = accelRate;
        }
        else {
            if (velocityX > 0) {
                accelerationX = -friction;
            } else if (velocityX < 0) {
                accelerationX = friction;
            } else {
                accelerationX = 0;
            }
        }

        if (invincibilityTimer > 0.0f) {
            invincibilityTimer -= deltaTime;
            if (invincibilityTimer < 0.0f)
                invincibilityTimer = 0.0f;
        }

        velocityX += accelerationX;

        if (velocityX > maxSpeedX) velocityX = maxSpeedX;
        if (velocityX < -maxSpeedX) velocityX = -maxSpeedX;

        if (abs(velocityX) < 0.1f && !isRunning) {
            velocityX = 0;
        }

        x += velocityX;
        y += velocityY;
        checkCollisions(level);

        characterSprite.setPosition(x, y);
    }

    bool isBoosted;

private:
    float defaultMaxSpeedX;
    float defaultAccelRate;
    float boostTimer;
    Texture texBoost;
    Animation boostAnim;
};

