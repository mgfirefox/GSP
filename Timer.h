#pragma once
#include <Windows.h>

class Timer
{
    bool initialized;
    bool released;

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;

    double deltaTime;

public:
    Timer();
    ~Timer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    double getDeltaTime();

    void initialize();
    void update();
    void release();
};
