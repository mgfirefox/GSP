#include "Application.h"

std::unique_ptr<Application> Application::application;

Application::Application() : renderer() {
    initialized = false;
    released = false;

    name = NULL;
    instanceHandle = NULL;

    classAtom = 0;

    shouldShutdown = false;
}

Application::~Application() {
    release();
}

bool Application::isInitialized() {
    return initialized;
}

void Application::setInitialized() {
    initialized = true;
    released = false;
}

bool Application::isReleased() {
    return released;
}

void Application::setReleased() {
    initialized = false;
    released = true;
}

std::unique_ptr<Application>& Application::getApplication() {
    if (!application) {
        application = std::make_unique<Application>();
    }

    return application;
}

bool Application::initialize(LPCWCHAR name, HINSTANCE instanceHandle, int showCmd) {
    if (isInitialized()) {
        release();
    }
    
    this->name = name;
    this->instanceHandle = instanceHandle;

    WNDCLASSEX windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = windowProcedure;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = this->instanceHandle;
    windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    windowClass.hIconSm = windowClass.hIcon;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = this->name;
    classAtom = RegisterClassEx(&windowClass);
    if (classAtom == 0) {
        return false;
    }

    window = std::make_unique<Window>();
    window->initialize(classAtom, this->name, this->instanceHandle, showCmd, FULLSCREEN_ENABLED);

    renderer = std::make_unique<Renderer>();
    bool result = renderer->initialize(window->getWidth(), window->getHeight(), window->getHandle());
    if (!result) {
        return false;
    }

    std::unique_ptr<Input>& input = Input::getInput();
    input->initialize();

    ActionFunction closeApplicationFunction = [=](std::string actionName, InputObject inputObject) {
        if (inputObject.inputState == InputState::END) {
            shouldShutdown = true;
        }
        };
    int keyCodes[] = { VK_ESCAPE };
    input->bindAction(CLOSE_APPLICATION_ACTION_NAME, closeApplicationFunction, keyCodes, 1, NULL, 0);

    setInitialized();
    return true;
}

void Application::run() {
    MSG windowMessage = {};

    std::unique_ptr<Timer>& timer = Timer::getTimer();
    timer->initialize();

    std::unique_ptr<Input>& input = Input::getInput();

    std::unique_ptr<FpsCounter>& fpsCounter = FpsCounter::getFpsCounter();
    fpsCounter->initialize();

    while (!shouldShutdown) {
        if (PeekMessage(&windowMessage, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&windowMessage);
            DispatchMessage(&windowMessage);
        }
        if (windowMessage.message == WM_QUIT) {
            break;
        }

        timer->update();

        input->onInputEvents();

        renderer->updateCamera();
        bool result = renderer->renderFrame();
        if (!result) {
            break;
        }

        wchar_t c[32];
        swprintf_s(c, 32, L"%d", fpsCounter->getFps());
        c[31] = '\0';
        OutputDebugString(L"\n\nFPS: ");
        OutputDebugString(c);

        fpsCounter->update();

        /*if (!VSYNC_ENABLED && FPS_LIMIT != 0) {
            fpsCounter->limit(FPS_LIMIT);
        }*/
    }
}

void Application::release() {
    if (isReleased()) {
        return;
    }

    std::unique_ptr<FpsCounter>& fpsCounter = FpsCounter::getFpsCounter();
    fpsCounter->release();
    fpsCounter.reset();

    std::unique_ptr<Timer>& timer = Timer::getTimer();
    timer->release();
    timer.reset();

    std::unique_ptr<Input>& input = Input::getInput();
    input->unbindAction(CLOSE_APPLICATION_ACTION_NAME);
    input->release();
    input.reset();

    renderer->release();
    renderer.reset();    

    window->release();
    window.reset();

    UnregisterClass(MAKEINTATOM(classAtom), instanceHandle);
    classAtom = 0;

    name = NULL;
    instanceHandle = NULL;

    shouldShutdown = false;

    setReleased();
}

LRESULT Application::windowProcedure(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
    return application->onAllWindowMessages(windowHandle, message, wparam, lparam);
}

LRESULT Application::onAllWindowMessages(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    // Window Notifications
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    // Keyboard Input Notifications
    case WM_KEYDOWN: {
        WORD keyFlags = HIWORD(lparam);
        if (keyFlags & KF_REPEAT) {
            break;
        }
    }
    case WM_KEYUP:


    // Mouse Input Notifications
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: {
        InputEvent inputEvent = {};
        inputEvent.message = message;
        inputEvent.wparam = wparam;
        inputEvent.lparam = lparam;

        std::unique_ptr<Input>& input = Input::getInput();
        input->addInputEvent(inputEvent);
        break;
    }

    // Raw Input Notifications
    /*case WM_INPUT:
        input->onRawInputNotifications(message, wparam, lparam);
        break;*/

    default: return DefWindowProc(windowHandle, message, wparam, lparam);
    }

    return 0;
}
