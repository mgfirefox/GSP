#pragma once
#include <Windows.h>

#include <memory>

#include "Timer.h"

#include "IntUtility.h"

class FpsCounter
{
    bool initialized;
    bool released;

    std::shared_ptr<Timer> timer;

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;
    int32 frameCount = 0;

    int32 fps;

public:
    FpsCounter(std::shared_ptr<Timer> timer);
    ~FpsCounter();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    int32 getFps();

    void initialize();
    void update();
    void release();

    void limit(uint32 targetFps);
};
