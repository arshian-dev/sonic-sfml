#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include "Level.h"
#include <cmath>
using namespace sf;
using namespace std;
class Character {

public:
    float invincibilityTimer = 0;
    SoundBuffer jumpSoundBuffer;
    Sound jumpSound;
    float x, y;
    float velocityX, velocityY;
    float accelerationX;
    float gravity = 0.4f;
    float maxSpeedX = 18.0f;
    float accelRate = 0.5f;
    float friction = 0.3f;
    bool isJumping;
    bool isRunning;
    int hp;

    float Pwidth, Pheight;

    Animation runAnim;
    Animation jumpAnim;
    Animation invincibleAnim;

    Sprite characterSprite;
    Texture texIdle, texRun, texJump, texinvincible;

    Character(Texture& texIdle, Texture& texRun, Texture& texJump, Texture& texinvincible, const Level& level, int runSprite, int jumpSprite, int /*sprintSprite*/) {
        this->texIdle = texIdle;
        this->texRun = texRun;
        this->texJump = texJump;
        this->texinvincible = texinvincible;

        characterSprite.setTexture(texIdle);
        characterSprite.setScale(1.5f, 1.5f);
        characterSprite.setOrigin(0, 0);

        int frameWidth = texRun.getSize().x / runSprite;
        int frameHeight = texRun.getSize().y;
        for (int i = 0; i < runSprite; i++) {
            runAnim.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }

        int jumpFrameWidth = texJump.getSize().x / jumpSprite;
        int jumpFrameHeight = texJump.getSize().y;
        for (int i = 0; i < jumpSprite; i++) {
            jumpAnim.addFrame(IntRect(i * jumpFrameWidth, 0, jumpFrameWidth, jumpFrameHeight));
        }

        int invincibleFrameWidth = texinvincible.getSize().x / runSprite;
        int invincibleFrameHeight = texinvincible.getSize().y;
        for (int i = 0; i < runSprite; i++) {
            invincibleAnim.addFrame(IntRect(i * invincibleFrameWidth, 0, invincibleFrameWidth, invincibleFrameHeight));
        }

        Pwidth = frameWidth * characterSprite.getScale().x;
        Pheight = frameHeight * characterSprite.getScale().y;

        x = 100;
        y = 0; // Defer y-position setting to caller
        velocityX = 0;
        velocityY = 0;
        accelerationX = 0;
        isJumping = false;
        isRunning = false;
        hp = 3;

        if (!jumpSoundBuffer.loadFromFile("assets/jump.wav")) {
            throw runtime_error("Failed to load jump.wav");
        }
        jumpSound.setBuffer(jumpSoundBuffer);
    }

    virtual ~Character() {}

    void setPosition(float newX, float newY) {
        x = newX;
        y = newY;
    }

    void decreaseHP(int amount) {
        if (hp > 0) hp -= amount;
        setInvincible(5.0f);
    }

    void applyPushback(float force) {
        velocityX = force;
    }

    void setInvincible(float time) {
        invincibilityTimer = time;
    }

    bool isInvincible() const {
        return invincibilityTimer > 0;
    }

    virtual void updateMovement(float deltaTime, const Level& level) {
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
            // No input, apply friction
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

        // Apply acceleration
        velocityX += accelerationX;

        // Clamp to max speed
        if (velocityX > maxSpeedX) velocityX = maxSpeedX;
        if (velocityX < -maxSpeedX) velocityX = -maxSpeedX;

        // Snap to stop if very slow
        if (abs(velocityX) < 0.1f && !isRunning) {
            velocityX = 0;
        }

        // Move horizontally
        x += velocityX;

        // Gravity and vertical movement
        y += velocityY;
        checkCollisions(level);

        // Animations
        if (isInvincible()) {
            invincibleAnim.update(deltaTime);
            characterSprite.setTexture(texinvincible);
            characterSprite.setTextureRect(invincibleAnim.getCurrentFrame());
        } else {
            if (isJumping || Keyboard::isKeyPressed(Keyboard::Down)) {
                jumpAnim.update(deltaTime);
                characterSprite.setTexture(texJump);
                characterSprite.setTextureRect(jumpAnim.getCurrentFrame());
            } else if (isRunning) {
                runAnim.update(deltaTime);
                characterSprite.setTexture(texRun);
                characterSprite.setTextureRect(runAnim.getCurrentFrame());
            }
            else {
                characterSprite.setTexture(texIdle);
                characterSprite.setTextureRect(IntRect(0, 0, texIdle.getSize().x, texIdle.getSize().y));
            }
        }
    }

    void applyGravity(const Level& level) {
        velocityY += gravity;
        float nextY = y + velocityY;

        int leftFoot = (int)(x + 2) / cell_size;
        int rightFoot = (int)(x + Pwidth - 2) / cell_size;
        int bottom = (int)(nextY + Pheight) / cell_size;

        bool touchingGround = false;
        for (int col = leftFoot; col <= rightFoot; col++) {
            if (bottom >= 0 && bottom < level.height && col >= 0 && col < level.width) {
                char tile = level.lvl[bottom][col];
                if (tile == 'w' || tile == 'q' || tile == 'e') {
                    touchingGround = true;
                    break;
                }
            }
        }

        if (touchingGround && velocityY >= 0) {
            isJumping = false;
            velocityY = 0;
            y = bottom * cell_size - Pheight;
        } else {
            y += velocityY;
        }
    }

    void jump() {
        if (!isJumping) {
            jumpSound.play();
            isJumping = true;
            velocityY = -10;
        }
    }

    void checkCollisions(const Level& level) {
        const int levelWidth = level.width;
        const int levelHeight = level.height;

        // Head collision (top)
        int headLeft = (int)(x + 2) / cell_size;
        int headRight = (int)(x + Pwidth - 2) / cell_size;
        int headY = (int)(y) / cell_size;

        for (int col = headLeft; col <= headRight; col++) {
            if (headY >= 0 && headY < levelHeight && col >= 0 && col < levelWidth) {
                char tile = level.lvl[headY][col];
                if (tile == 'w' || tile == 'e') { // Exclude 'q'
                    velocityY = 0;
                    y = (headY + 1) * cell_size;
                    break;
                }
            }
        }

        // Right collision
        int right = (int)(x + Pwidth - 1) / cell_size;
        int top = (int)(y + 2) / cell_size;
        int bottom = (int)(y + Pheight - 2) / cell_size;

        for (int row = top; row <= bottom; row++) {
            if (row >= 0 && row < levelHeight && right >= 0 && right < levelWidth) {
                char tile = level.lvl[row][right];
                if (tile == 'w' || tile == 'e' || tile == 'q') { // Include 'q'
                    x = right * cell_size - Pwidth;
                    velocityX = 0;
                    break;
                }
            }
        }

        // Left collision
        int left = (int)(x + 1) / cell_size;
        for (int row = top; row <= bottom; row++) {
            if (row >= 0 && row < levelHeight && left >= 0 && left < levelWidth) {
                char tile = level.lvl[row][left];
                if (tile == 'w' || tile == 'e' || tile == 'q') { // Include 'q'
                    x = (left + 1) * cell_size;
                    velocityX = 0;
                    break;
                }
            }
        }
    }

    void draw(RenderWindow& window) {
        characterSprite.setPosition(x, y);
        window.draw(characterSprite);
    }
};

