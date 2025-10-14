#include "Timer.h"

Timer::Timer()
{
    initialized = false;
    released = false;

    frequency = {};
    lastTime = {};

    deltaTime = 0.0f;
}

Timer::~Timer()
{
    release();
}

bool Timer::isInitialized()
{
    return initialized;
}

void Timer::setInitialized()
{
    initialized = true;
    released = false;
}

bool Timer::isReleased()
{
    return released;
}

void Timer::setReleased()
{
    initialized = false;
    released = true;
}

double Timer::getDeltaTime()
{
    return deltaTime;
}

void Timer::initialize()
{
    if (isInitialized())
    {
        release();
    }

    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    setInitialized();
}

void Timer::update()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    deltaTime = (currentTime.QuadPart - lastTime.QuadPart) / static_cast<double>(frequency.
        QuadPart);

    lastTime = currentTime;
}

void Timer::release()
{
    if (isReleased())
    {
        return;
    }

    deltaTime = 0.0f;

    lastTime = {};
    frequency = {};

    setReleased();
}
