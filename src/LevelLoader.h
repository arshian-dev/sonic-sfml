#pragma once

#include <cstdio>
#include <cstring>
using namespace sf;
using namespace std;

const int MAX_LEVEL_HEIGHT = 100;
const int MAX_LEVEL_WIDTH = 400;

inline bool loadLevelFromFile(const char* filename, char**& map, int& width, int& height) {
    FILE* file = fopen(filename, "r");
    if (!file) return false;

    char temp[MAX_LEVEL_HEIGHT][MAX_LEVEL_WIDTH];
    char line[MAX_LEVEL_WIDTH + 2];
    height = 0;
    width = 0;

    while (fgets(line, sizeof(line), file) && height < MAX_LEVEL_HEIGHT) {
        int len = strlen(line);
        if (line[len - 1] == '\n') line[len - 1] = '\0';
        len = strlen(line);
        if (len > width) width = len;

        for (int i = 0; i < len; ++i) {
            temp[height][i] = line[i];
        }
        for (int i = len; i < MAX_LEVEL_WIDTH; ++i) {
            temp[height][i] = ' ';
        }
        ++height;
    }

    fclose(file);

    map = new char*[height];
    for (int i = 0; i < height; ++i) {
        map[i] = new char[width];
        for (int j = 0; j < width; ++j) {
            map[i][j] = temp[i][j];
        }
    }

    return true;
}

inline void freeLevelMemory(char** map, int height) {
    if (!map) return;
    for (int i = 0; i < height; ++i) {
        delete[] map[i];
    }
    delete[] map;
}