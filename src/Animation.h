#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Animation {
public:
    static const int MAX_FRAMES = 20;

    Animation() {
        frameCount = 0;
        currentFrame = 0;
        frameTime = 0.1f;
        currentTime = 0.f;
    }

    void setFrameTime(float time) {
        frameTime = time;
    }

    void addFrame(const IntRect& rect) {
        if (frameCount < MAX_FRAMES) {
            frames[frameCount++] = rect;
        }
    }

    void update(float deltaTime) {
        if (frameCount <= 1) return;

        currentTime += deltaTime;
        if (currentTime >= frameTime) {
            currentTime = 0.f;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }

    IntRect getCurrentFrame() const {
        if (frameCount == 0) return IntRect();
        return frames[currentFrame];
    }

    void reset() {
        currentFrame = 0;
        currentTime = 0.f;
    }

private:
    IntRect frames[MAX_FRAMES];
    int frameCount;
    int currentFrame;
    float frameTime;
    float currentTime;
};

