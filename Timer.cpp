#include "Timer.h"

std::unique_ptr<Timer> Timer::timer;

Timer::Timer() {
    initialized = false;
    released = false;

    frequency = {};
    lastTime = {};

    deltaTime = 0.0f;
}

Timer::~Timer() {
    release();
}

bool Timer::isInitialized() {
    return initialized;
}

void Timer::setInitialized() {
    initialized = true;
    released = false;
}

bool Timer::isReleased() {
    return released;
}

void Timer::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<Timer>& Timer::getTimer() {
    if (!timer) {
        timer = std::make_unique<Timer>();
    }

    return timer;
}

float Timer::getDeltaTime() {
    return deltaTime;
}

void Timer::initialize() {
    if (isInitialized()) {
        release();
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    setInitialized();
}

void Timer::update() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

    lastTime = currentTime;
}

void Timer::release() {
    if (isReleased()) {
        return;
    }

    deltaTime = 0.0f;

    lastTime = {};
    frequency = {};

    setReleased();
}
