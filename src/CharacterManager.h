#pragma once

#include "Character.h"
using namespace sf;
using namespace std;

class CharacterManager {
private:
    Character* characters[3];
    int currentIndex;

    static const int MAX_TRAIL = 60;
    float trailX[MAX_TRAIL];
    float trailY[MAX_TRAIL];
    bool jumpTrail[MAX_TRAIL];
    int trailIndex;

public:
    CharacterManager() {
        currentIndex = 0;
        trailIndex = 0;
        for (int i = 0; i < MAX_TRAIL; i++) {
            trailX[i] = 0;
            trailY[i] = 0;
            jumpTrail[i] = false;
        }
    }

    void init(Character* sonic, Character* tails, Character* knuckles) {
        characters[0] = sonic;
        characters[1] = knuckles;
        characters[2] = tails;
    }

    void switchCharacter() {
        int nextIndex = (currentIndex + 1) % 3;
        Character* current = characters[currentIndex];
        Character* next = characters[nextIndex];

        next->setPosition(current->x, current->y);
        next->velocityY = current->velocityY;
        next->isJumping = current->isJumping;

        currentIndex = nextIndex;
    }

    Character* getCurrentCharacter() {
        return characters[currentIndex];
    }

    void updateTrail() {
        Character* current = getCurrentCharacter();
        trailX[trailIndex] = current->x;
        trailY[trailIndex] = current->y;
        jumpTrail[trailIndex] = current->isJumping;
        trailIndex = (trailIndex + 1) % MAX_TRAIL;
    }

    void updateFollowers() {
        int offset1 = (trailIndex + MAX_TRAIL - 5) % MAX_TRAIL;
        int offset2 = (trailIndex + MAX_TRAIL - 10) % MAX_TRAIL;

        for (int i = 0; i < 3; i++) {
            if (i == currentIndex) continue;

            int offset = (i == (currentIndex + 1) % 3) ? offset1 : offset2;

            characters[i]->x += (trailX[offset] - characters[i]->x) * 0.2f;

            // Trigger jump if leader jumped at that offset
            if (jumpTrail[offset]) {
                characters[i]->jump(); // internally checks if jump is allowed
            }
        }
    }

    void updateAll(float deltaTime, const Level& level, RenderWindow& window) {
        for (int i = 0; i < 3; i++) {
            characters[i]->updateMovement(deltaTime, level);
            characters[i]->applyGravity(level);
            characters[i]->checkCollisions(level);

            if (characters[i]->y > window.getSize().y + 100) {
                Character* leader = getCurrentCharacter();
                characters[i]->setPosition(leader->x - 50, leader->y);
                
            }
        }

        updateTrail();
        updateFollowers();
    }

    void drawAll(RenderWindow& window) {
        for (int i = 0; i < 3; i++) {
            characters[i]->draw(window);
        }
        getCurrentCharacter()->draw(window);
    }
};

