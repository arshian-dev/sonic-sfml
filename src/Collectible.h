#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Character.h"
using namespace sf;
using namespace std;

class Collectible {
protected:
    Sprite sprite;
    Texture texture;
    Animation animation;
    float x, y;
    bool collected = false;

public:
    Collectible(float posX, float posY) : x(posX), y(posY) {}

    virtual ~Collectible() {}

    virtual void update(float deltaTime) {
        if (!collected) {
            animation.update(deltaTime);
            sprite.setTextureRect(animation.getCurrentFrame());
        }
    }

    virtual void draw(RenderWindow& window) {
        if (!collected) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    virtual bool checkCollision(Character& character) {
        if (collected) return false;

        FloatRect myBounds(x, y, sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
        FloatRect playerBounds(character.x, character.y, character.Pwidth, character.Pheight);

        if (myBounds.intersects(playerBounds)) {
            collected = true;
            onCollect(character); // 🎯
            return true;
        }
        return false;
    }

    virtual void onCollect(Character& character) = 0; //Must be overridden
    
    bool isCollected() const {
        return collected;
    }
};

