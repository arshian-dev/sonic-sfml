#pragma once
#include "Character.h"
using namespace sf;
using namespace std;

class Knuckles : public Character {
private:
    Texture texPunch;
    Animation punchAnim;

public:
    bool isPunching;
    Knuckles(Texture& texIdle, Texture& texRun, Texture& texJump, Texture& texPunch, Texture& texinvincible, const Level& level, int runSprite, int jumpSprite, int sprintSprite)
        : Character(texIdle, texRun, texJump, texinvincible, level, runSprite, jumpSprite, sprintSprite), isPunching(false)
    {
        maxSpeedX = 12.0f;
        // Load punch texture
        this->texPunch = texPunch;
        int frameWidth = texPunch.getSize().x / 4;
        int frameHeight = texPunch.getSize().y;
        for (int i = 0; i < 4; i++) {
            punchAnim.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }
    }

    bool punch() {
        isPunching = true;
        return true; // Indicates punch was triggered
    }

    void updateMovement(float deltaTime, const Level& level) override {
        Character::updateMovement(deltaTime, level);
        isPunching = false; // Reset punch state each frame

        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            punch();
        }

        // Animation handling
        if (isInvincible()) {
            invincibleAnim.update(deltaTime);
            characterSprite.setTexture(texinvincible);
            characterSprite.setTextureRect(invincibleAnim.getCurrentFrame());
        }
        else if (isPunching) {
            punchAnim.update(deltaTime);
            characterSprite.setTexture(texPunch);
            characterSprite.setTextureRect(punchAnim.getCurrentFrame());
        }
        else if (isJumping || Keyboard::isKeyPressed(Keyboard::Down)) {
            jumpAnim.update(deltaTime);
            characterSprite.setTexture(texJump);
            characterSprite.setTextureRect(jumpAnim.getCurrentFrame());
        }
        else if (isRunning) {
            runAnim.update(deltaTime);
            characterSprite.setTexture(texRun);
            characterSprite.setTextureRect(runAnim.getCurrentFrame());
        }
        else {
            characterSprite.setTexture(texIdle);
            characterSprite.setTextureRect(IntRect(0, 0, texIdle.getSize().x, texIdle.getSize().y));
        }
    }
};
