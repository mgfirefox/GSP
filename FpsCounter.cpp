#include "FpsCounter.h"

std::unique_ptr<FpsCounter> FpsCounter::fpsCounter;

FpsCounter::FpsCounter() {
    initialized = false;
    released = false;

    frequency = {};
    lastTime = {};
    frameCount = 0;

    fps = 0;
}

FpsCounter::~FpsCounter() {
    release();
}

bool FpsCounter::isInitialized() {
    return initialized;
}

void FpsCounter::setInitialized() {
    initialized = true;
    released = false;
}

bool FpsCounter::isReleased() {
    return released;
}

void FpsCounter::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<FpsCounter>& FpsCounter::getFpsCounter() {
    if (!fpsCounter) {
        fpsCounter = std::make_unique<FpsCounter>();
    }

    return fpsCounter;
}

int FpsCounter::getFps() {
    return fps;
}

void FpsCounter::initialize() {
    if (isInitialized()) {
        release();
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    setInitialized();
}

void FpsCounter::update() {
    frameCount++;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    float elapsedTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
    if (elapsedTime < 1.0f) {
        return;
    }
    
    fps = frameCount;
    frameCount = 0;

    lastTime = currentTime;
}

//void FpsCounter::limit(int targetFps) {
//    float targetDeltaTime = 1.0f / targetFps;
//
//    std::unique_ptr<Timer>& timer = Timer::getTimer();
//    float frameTime = timer->getDeltaTime();
//    if (frameTime < targetDeltaTime) {
//        DWORD sleepTime = (DWORD)((targetDeltaTime - frameTime) * 1000);
//        Sleep(sleepTime);
//    }
//}

void FpsCounter::release() {
    if (isReleased()) {
        return;
    }

    fps = 0;

    frameCount = 0;
    lastTime = {};
    frequency = {};

    setReleased();
}
