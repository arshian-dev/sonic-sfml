#pragma once
using namespace sf;
using namespace std;

class ScoreManager {
private:
    int score;

public:
    ScoreManager() : score(0) {}

    void addScore(int points) {
        score += points;
    }

    int getScore() const {
        return score;
    }

    void setScore(int scoreP){
        score = scoreP;
    }
    void reset() {
        score = 0;
    }
};

