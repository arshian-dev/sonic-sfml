#pragma once

#include <SFML/Graphics.hpp>
#include "LevelLoader.h"
using namespace std;
using namespace sf;

const int cell_size = 64;

class Level {
public:
    char** lvl;
    int width;
    int height;

    Level() : lvl(nullptr), width(0), height(0) {}

    ~Level() {
        if (lvl) {
            freeLevelMemory(lvl, height);
        }
    }

    bool loadFromFile(const char* filename) {
        if (lvl) {
            freeLevelMemory(lvl, height);
            lvl = nullptr;
            width = 0;
            height = 0;
        }
        return loadLevelFromFile(filename, lvl, width, height);
    }

    void draw(RenderWindow& window, Sprite& wall1, Sprite& wall2, Sprite& wall3, Sprite& tempBlock) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (lvl[i][j] == 'w') {
                    wall2.setPosition(j * cell_size, i * cell_size);
                    window.draw(wall2);
                } else if (lvl[i][j] == 'q') {
                    wall1.setPosition(j * cell_size, i * cell_size);
                    window.draw(wall1);
                } else if (lvl[i][j] == 'e') {
                    wall3.setPosition(j * cell_size, i * cell_size);
                    window.draw(wall3);
                } else if (lvl[i][j] == 'd') {
                    wall3.setPosition(j * cell_size, i * cell_size);
                    window.draw(wall3);
                } else if (lvl[i][j] == 't') {
                    tempBlock.setPosition(j * cell_size, i * cell_size);
                    window.draw(tempBlock);
                }
                // Note: 'x' and 'l' blocks are not drawn
            }
        }
    }

    bool checkEndBlockCollision(float x, float y, float charWidth, float charHeight) const {
        // Convert character position to cell coordinates
        int leftCell = static_cast<int>(x / cell_size);
        int rightCell = static_cast<int>((x + charWidth) / cell_size);
        int topCell = static_cast<int>(y / cell_size);
        int bottomCell = static_cast<int>((y + charHeight) / cell_size);

        // Ensure cell indices are within bounds
        leftCell = max(0, min(leftCell, width - 1));
        rightCell = max(0, min(rightCell, width - 1));
        topCell = max(0, min(topCell, height - 1));
        bottomCell = max(0, min(bottomCell, height - 1));

        // Check if any part of the character overlaps with an 'x' block
        for (int i = topCell; i <= bottomCell; ++i) {
            for (int j = leftCell; j <= rightCell; ++j) {
                if (lvl[i][j] == 'x') {
                    return true;
                }
            }
        }
        return false;
    }

    bool checkTriggerBlockCollision(float x, float y, float charWidth, float charHeight) const {
        // Convert character position to cell coordinates
        int leftCell = static_cast<int>(x / cell_size);
        int rightCell = static_cast<int>((x + charWidth) / cell_size);
        int topCell = static_cast<int>(y / cell_size);
        int bottomCell = static_cast<int>((y + charHeight) / cell_size);

        // Ensure cell indices are within bounds
        leftCell = max(0, min(leftCell, width - 1));
        rightCell = max(0, min(rightCell, width - 1));
        topCell = max(0, min(topCell, height - 1));
        bottomCell = max(0, min(bottomCell, height - 1));

        // Check if any part of the character overlaps with an 'l' block
        for (int i = topCell; i <= bottomCell; ++i) {
            for (int j = leftCell; j <= rightCell; ++j) {
                if (lvl[i][j] == 'l') {
                    return true;
                }
            }
        }
        return false;
    }

    void clearTempBlocks() {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (lvl[i][j] == 't') {
                    lvl[i][j] = ' ';
                }
            }
        }
    }

    int getGroundLevel() const {
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                if (lvl[i][j] == 'w') {
                    return i;
                }
            }
        }
        return height - 1;
    }
};
