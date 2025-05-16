#pragma once
#include <iostream>
#include "Collectible.h"
using namespace sf;
using namespace std;

class ExtraLife : public Collectible {
public:
    ExtraLife(float posX, float posY) : Collectible(posX, posY) {
        if (!texture.loadFromFile("assets/extra_life.png")) {
            cerr<<"Failed to load texture: assets/extra_life.png";
        }
        sprite.setTexture(texture);
        sprite.setOrigin(16, 16);
    }

    void update(float deltaTime) override {
        //no need for animation
    }

    void onCollect(Character& character) override {
        character.hp += 1; //(increase HP by 1)
    }
};