#pragma once

#include "Character.h"
#include "CharacterManager.h"
using namespace sf;
using namespace std;

class Tails : public Character {
private:
    bool isFlying = false;
    float flightTimer = 0.0f;
    float flightStartY = 0.0f;
    const float maxFlightDuration = 7.0f; //flight time
    const float flightSpeed = -8.0f; 
    const float maxFlightHeight = 7.0f * 64.0f;
    Texture texFly;
    Animation flyAnim;
    CharacterManager& characterManager; // Reference to CharacterManager

public:
    Tails(Texture& texIdle, Texture& texRun, Texture& texJump, Texture& texFly, Texture& texinvincible, const Level& level, int runSprite, int jumpSprite, int sprintSprite, CharacterManager& cm)
        : Character(texIdle, texRun, texJump, texinvincible, level, runSprite, jumpSprite, sprintSprite), 
          texFly(texFly), 
          characterManager(cm) {
        maxSpeedX = 10.0f;
        // Initialize flying animation
        int flyFrameWidth = texFly.getSize().x / jumpSprite;
        int flyFrameHeight = texFly.getSize().y;
        for (int i = 0; i < jumpSprite; i++) {
            flyAnim.addFrame(IntRect(i * flyFrameWidth, 0, flyFrameWidth, flyFrameHeight));
        }
    }

    void updateMovement(float deltaTime, const Level& level) override {
        Character::updateMovement(deltaTime, level);

        if (!isJumping && !isFlying) {
            flightTimer = 0.0f;
        }

        //flight input only if Tails is the current character
        if (characterManager.getCurrentCharacter() == this && Keyboard::isKeyPressed(Keyboard::Space) && flightTimer < maxFlightDuration) {
            if (!isFlying && !isJumping) {
                isFlying = true;
                isJumping = true;
                velocityY = flightSpeed;
                flightStartY = y;
                jumpSound.play();
            } else if (isFlying) {
                if (flightStartY - y < maxFlightHeight) {
                    velocityY = flightSpeed;
                } else {
                    velocityY = 0.0f;
                }
                flightTimer += deltaTime;
            }
        } else if (isFlying) {

            isFlying = false;
        }

        if (isFlying) {
            flyAnim.update(deltaTime);
            characterSprite.setTexture(texFly);
            characterSprite.setTextureRect(flyAnim.getCurrentFrame());
        }
    }
};
