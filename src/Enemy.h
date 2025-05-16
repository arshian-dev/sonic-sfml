#pragma once
#include <SFML/Graphics.hpp>
#include "Character.h"
#include "Animation.h"
using namespace sf;
using namespace std;
class Enemy {
protected:
    Sprite sprite;
    Texture texIdle, texRun;
    float x, y;
    int hp;
    float Ewidth, Eheight;

public:
    Animation runAnim;

    Enemy(Texture& texIdle, Texture& texRun, float xPos, float yPos, int initialHP, int runSprite)
        : hp(initialHP) {
        this->texIdle = texIdle;
        this->texRun = texRun;

        sprite.setTexture(texIdle);
        sprite.setScale(1.5f, 1.5f);
        sprite.setOrigin(0, 0);

        int frameWidth = texRun.getSize().x / runSprite;
        int frameHeight = texRun.getSize().y;
        for (int i = 0; i < runSprite; i++) {
            runAnim.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }

        Ewidth = frameWidth * sprite.getScale().x;
        Eheight = frameHeight * sprite.getScale().y;

        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
    }

    virtual ~Enemy() {}

    virtual bool isAlive() const { return hp > 0; }

    virtual void update(float deltaTime) = 0;
    virtual void draw(RenderWindow& window) = 0;
    virtual void checkCollision(Character& character) = 0;
    virtual FloatRect getBounds() const = 0;

    void setPosition(float xPos, float yPos) {
        x = xPos;
        y = yPos;
        sprite.setPosition(x, y);
    }

    float getX() const { return x; }
    float getY() const { return y; }

    int gethp() const { return hp; }

    void setX(float xPos) { x = xPos; sprite.setPosition(x, y); }
    void setY(float yPos) { y = yPos; sprite.setPosition(x, y); }

protected:
    void updateAnimation(float deltaTime, bool isMoving) {
        if (isMoving) {
            runAnim.update(deltaTime);
            sprite.setTexture(texRun);
            sprite.setTextureRect(runAnim.getCurrentFrame());
        } else {
            sprite.setTexture(texIdle);
            sprite.setTextureRect(IntRect(0, 0, texIdle.getSize().x, texIdle.getSize().y));
        }
    }
};
