#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Character.h"
using namespace sf;
using namespace std;

class Obstacle {
protected:
    Texture texture;
    Sprite sprite;
    Animation animation;
    float x, y;

public:
    Obstacle(float posX, float posY) : x(posX), y(posY) {
        // Texture and sprite will be initialized by derived classes
    }

    virtual ~Obstacle() {}

    virtual void update(float deltaTime) {
        animation.update(deltaTime);
        sprite.setTextureRect(animation.getCurrentFrame());
    }

    virtual void draw(RenderWindow& window) {
        sprite.setPosition(x, y);
        window.draw(sprite);
    }

    virtual void checkCollision(Character& character) {
        // Base obstacle doesn't cause damage or affect character by default
        sprite.setPosition(x, y);
        FloatRect obstacleBounds = sprite.getGlobalBounds();
        FloatRect playerBounds(character.x, character.y, character.Pwidth, character.Pheight);

        if (obstacleBounds.intersects(playerBounds)) {
            // Basic collision: Treat as a wall for side collisions
            float playerCenter = character.x + character.Pwidth / 2;
            float obstacleCenter = x + obstacleBounds.width / 2;
            if (playerCenter < obstacleCenter) {
                // Character hits from the left
                character.x = x - character.Pwidth;
                if (character.velocityX > 0) {
                    character.velocityX = 0;
                }
            } else {
                // Character hits from the right
                character.x = x + obstacleBounds.width;
                if (character.velocityX < 0) {
                    character.velocityX = 0;
                }
            }
        }
    }
};