#pragma once
#include <Windows.h>

#include <memory>

#include <thread>

#include "Timer.h"

class FpsCounter {
private:
    bool initialized;
    bool released;

    static std::unique_ptr<FpsCounter> fpsCounter;

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;
    int frameCount = 0;

    int fps;

public:
    FpsCounter();
    ~FpsCounter();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    static std::unique_ptr<FpsCounter>& getFpsCounter();

    int getFps();

    void initialize();
    void update();
    //void limit(int targetFps);
    void release();
};

