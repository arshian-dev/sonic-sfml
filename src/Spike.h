#pragma once

#include "Obstacle.h"
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "Character.h"
using namespace sf;
using namespace std;

class Spike : public Obstacle {
public:
    Spike(float posX, float posY) : Obstacle(posX, posY) {
        if (!texture.loadFromFile("assets/spike.png")) {
            throw runtime_error("Failed to load spike.png");
        }
        sprite.setTexture(texture);
        sprite.setScale(1.0f, 1.0f);

        int frameWidth = texture.getSize().x / 4;
        int frameHeight = texture.getSize().y;

        for (int i = 0; i < 4; i++) {
            animation.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }
    }

    void checkCollision(Character& character) override {
        sprite.setPosition(x, y);
        FloatRect spikeBounds = sprite.getGlobalBounds();
        FloatRect playerBounds(character.x, character.y, character.Pwidth, character.Pheight);

        if (spikeBounds.intersects(playerBounds)) {
            float playerBottom = character.y + character.Pheight;
            float spikeTop = y;
            float verticalOverlap = playerBottom - spikeTop;

            if (verticalOverlap > 0 && verticalOverlap < 15.0f && character.velocityY > 0) {
                // Top collision: Land on spike
                character.y = y - character.Pheight;
                character.velocityY = 0;
                character.isJumping = false;
                if (!character.isInvincible()) {
                    character.decreaseHP(1);
                    character.setInvincible(2.0f);
                    float charCenter = character.x + character.Pwidth / 2;
                    float spikeCenter = x + spikeBounds.width / 2;
                    float pushback = (charCenter < spikeCenter) ? -10.0f : 10.0f;
                    character.applyPushback(pushback);
                }
            } else {
                // Side collision: Delegate to base class for wall-like behavior
                Obstacle::checkCollision(character);
            }
        }
    }
};