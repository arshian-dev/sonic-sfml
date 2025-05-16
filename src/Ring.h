#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include "Collectible.h"
using namespace sf;
using namespace std;

class Ring : public Collectible {
private:
    Animation animation;
    SoundBuffer soundBuffer;
    Sound sound;

public:
    Ring(float posX, float posY) : Collectible(posX, posY) {
        if (!texture.loadFromFile("assets/ring.png")) {
            throw runtime_error("Failed to load ring.png");
        }
        sprite.setTexture(texture);
        sprite.setScale(2.0f, 2.0f);

        int frameWidth = texture.getSize().x / 4;
        int frameHeight = texture.getSize().y;

        for (int i = 0; i < 4; i++) {
            animation.addFrame(IntRect(i * frameWidth, 0, frameWidth, frameHeight));
        }

        if (!soundBuffer.loadFromFile("assets/ring_collect.wav")) {
            throw runtime_error("Failed to load ring_collect.wav");
        }
        sound.setBuffer(soundBuffer);
    }

    void update(float deltaTime) override {
        if (!collected) {
            animation.update(deltaTime);
            sprite.setTextureRect(animation.getCurrentFrame());
        }
    }

    void draw(RenderWindow& window) override {
        if (!collected) {
            sprite.setPosition(x, y);
            window.draw(sprite);
        }
    }

    bool checkCollision(Character& character) override {
        if (collected) return false;

        FloatRect ringBounds(x, y, sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
        FloatRect playerBounds(character.x, character.y, character.Pwidth, character.Pheight);

        if (ringBounds.intersects(playerBounds)) {
            collected = true;
            onCollect(character);
            return true;
        }
        return false;
    }

    void onCollect(Character& character) override {
        sound.play();
        // No specific action needed for rings; totalRings is incremented in Game
    }
};