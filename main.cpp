#include <Windows.h>

#include <memory>

#include "Input.h"
#include "Timer.h"
#include "FpsCounter.h"

#include "Application.h"

#include "MemoryUtility.h"

int32 WINAPI WinMain(HINSTANCE applicationInstanceHandle,
                     HINSTANCE prevApplicationInstanceHandle,
                     char* cmdLine, int32 showCmd)
{
    std::shared_ptr<Input> input = createSharedPointer<Input>();
    input->initialize();

    std::shared_ptr<Timer> timer = createSharedPointer<Timer>();
    timer->initialize();

    std::shared_ptr<FpsCounter> fpsCounter = createSharedPointer<FpsCounter>(timer);
    fpsCounter->initialize();

    std::shared_ptr<Application> application = createSharedPointer<Application>(
        input, timer, fpsCounter);
    bool result = application->initialize(applicationInstanceHandle, L"GSP", application, showCmd);

    if (result)
    {
        application->run();
    }

    application.reset();

    fpsCounter.reset();

    timer.reset();

    input.reset();

    return 0;
}
