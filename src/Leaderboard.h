#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <cstring> // for strncpy, strcmp

using namespace std;
using namespace sf;

class Leaderboard {
private:
    static const int MAX_ENTRIES = 5;
    static const int MAX_NAME_LENGTH = 11;

    struct ScoreEntry {
        char name[MAX_NAME_LENGTH];
        int score;
    };

    ScoreEntry entries[MAX_ENTRIES];
    string filename;

    void sortEntries() {
        for (int i = 0; i < MAX_ENTRIES - 1; ++i) {
            for (int j = 0; j < MAX_ENTRIES - i - 1; ++j) {
                if (entries[j].score < entries[j + 1].score) {
                    swap(entries[j], entries[j + 1]);
                }
            }
        }
    }

    void resetEntries() {
        for (int i = 0; i < MAX_ENTRIES; ++i) {
            strncpy(entries[i].name, "PLAYER", MAX_NAME_LENGTH);
            entries[i].name[MAX_NAME_LENGTH - 1] = '\0';
            entries[i].score = 0;
        }
    }

public:
    Leaderboard(const string& file = "leaderboard.txt") : filename(file) {
        resetEntries();
        load();
    }

    void load() {
        resetEntries(); //clearing previous entry if duplicate

        ifstream inFile(filename.c_str());
        if (inFile.is_open()) {
            int i = 0;
            char buffer[MAX_NAME_LENGTH];
            int score;

            while (i < MAX_ENTRIES && inFile >> buffer >> score) {
                strncpy(entries[i].name, buffer, MAX_NAME_LENGTH);
                entries[i].name[MAX_NAME_LENGTH - 1] = '\0';
                entries[i].score = score;
                ++i;
            }

            inFile.close();
            sortEntries();
        }
    }

    void save() {
        ofstream outFile(filename.c_str());
        if (outFile.is_open()) {
            for (int i = 0; i < MAX_ENTRIES; ++i) {
                outFile << entries[i].name << " " << entries[i].score << "\n";
            }
            outFile.close();
        }
    }

    void addScore(const string& name, int score) {
        for (int i = 0; i < MAX_ENTRIES; ++i) {
            if (strncmp(entries[i].name, name.c_str(), MAX_NAME_LENGTH) == 0 && entries[i].score == score) {
                return; //duplicate entry
            }
        }

        int lowestIndex = 0;
        int lowestScore = entries[0].score;

        for (int i = 1; i < MAX_ENTRIES; ++i) {
            if (entries[i].score < lowestScore) {
                lowestScore = entries[i].score;
                lowestIndex = i;
            }
        }

        if (score > lowestScore) {
            strncpy(entries[lowestIndex].name, name.c_str(), MAX_NAME_LENGTH);
            entries[lowestIndex].name[MAX_NAME_LENGTH - 1] = '\0';
            entries[lowestIndex].score = score;

            sortEntries();
            save();
        }
    }

    void draw(RenderWindow& window, Font& font, float x, float y) {
        Text text;
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(Color::White);
        text.setString("Leaderboard:");
        text.setPosition(x, y);
        window.draw(text);

        for (int i = 0; i < MAX_ENTRIES; ++i) {
            string display = string(entries[i].name) + " " + to_string(entries[i].score);
            text.setString(display);
            text.setPosition(x, y + 40 + i * 40);
            window.draw(text);
        }
    }
};
