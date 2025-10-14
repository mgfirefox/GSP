#include "FpsCounter.h"

FpsCounter::FpsCounter(std::shared_ptr<Timer> timer)
{
    initialized = false;
    released = false;

    this->timer = timer;

    frequency = {};
    lastTime = {};
    frameCount = 0;

    fps = 0;
}

FpsCounter::~FpsCounter()
{
    release();

    timer.reset();
}

bool FpsCounter::isInitialized()
{
    return initialized;
}

void FpsCounter::setInitialized()
{
    initialized = true;
    released = false;
}

bool FpsCounter::isReleased()
{
    return released;
}

void FpsCounter::setReleased()
{
    initialized = false;
    released = true;
}

int32 FpsCounter::getFps()
{
    return fps;
}

void FpsCounter::initialize()
{
    if (isInitialized())
    {
        release();
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    setInitialized();
}

void FpsCounter::update()
{
    frameCount++;

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    double elapsedTime = (currentTime.QuadPart - lastTime.QuadPart) / static_cast<double>(frequency.
        QuadPart);
    if (elapsedTime < 1.0)
    {
        return;
    }

    fps = frameCount;
    frameCount = 0;

    lastTime = currentTime;
}

void FpsCounter::release()
{
    if (isReleased())
    {
        return;
    }

    fps = 0;

    frameCount = 0;
    lastTime = {};
    frequency = {};

    setReleased();
}

void FpsCounter::limit(uint32 targetFps)
{
    double deltaTime = timer->getDeltaTime();
    double targetDeltaTime = 1.0f / targetFps;
    if (deltaTime < targetDeltaTime)
    {
        uint32 sleepTime = static_cast<uint32>((targetDeltaTime - deltaTime) * 1000);
        Sleep(sleepTime);
    }
}
