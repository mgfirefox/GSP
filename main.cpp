#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <memory>

#include "Application.h"

int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE prevApplicationInstanceHandle, LPSTR cmdLine, int showCmd) {
    std::unique_ptr<Application>& application = Application::getApplication();
    bool result = application->initialize(L"GSP LW3", applicationInstanceHandle, showCmd);
    if (result) {
        application->run();
    }
    application.release();
    application.reset();
    
    return 0;
}
