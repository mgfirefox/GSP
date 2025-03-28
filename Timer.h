#pragma once
#include <Windows.h>

#include <memory>

class Timer {
    bool initialized;
    bool released;

    static std::unique_ptr<Timer> timer;

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;

    float deltaTime;

public:
    Timer();
    ~Timer();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    static std::unique_ptr<Timer>& getTimer();

    float getDeltaTime();

    void initialize();
    void update();
    void release();
};

