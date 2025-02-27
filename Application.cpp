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
    if (!classAtom) {
        return false;
    }

    window = std::make_unique<Window>();
    window->initialize(classAtom, this->name, this->instanceHandle, showCmd, FULLSCREEN_ENABLED);

    std::unique_ptr<Input>& input = Input::getInput();
    input->initialize();

    ActionFunction closeApplicationFunction = [=](std::string actionName, InputObject inputObject) {
        if (inputObject.inputState == InputState::END) {
            shouldShutdown = true;
        }
        };
    int keyCodes[] = { VK_ESCAPE };
    input->bindAction(CLOSE_APPLICATION_ACTION_NAME, closeApplicationFunction, keyCodes, 1);

    renderer = std::make_unique<Renderer>();
    bool result = renderer->initialize(window->getWidth(), window->getHeight(), window->getHandle());
    if (!result) {
        return false;
    }

    setInitialized();
    return true;
}

void Application::run() {
    MSG windowMessage = {};

    while (!shouldShutdown) {
        if (PeekMessage(&windowMessage, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&windowMessage);
            DispatchMessage(&windowMessage);
        }
        if (windowMessage.message == WM_QUIT) {
            break;
        }
        if (shouldShutdown) {
            break;
        }
        else {
            shouldShutdown = !renderer->renderFrame();
        }
    }
}

void Application::release() {
    if (isReleased()) {
        return;
    }

    renderer->release();
    renderer.reset();

    std::unique_ptr<Input>& input = Input::getInput();
    input->unbindAction(CLOSE_APPLICATION_ACTION_NAME);
    input->release();
    input.reset();

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
    return application->onWindowMessages(windowHandle, message, wparam, lparam);
}

LRESULT Application::onWindowMessages(HWND windowHandle, UINT message, WPARAM wparam, LPARAM lparam) {
    std::unique_ptr<Input>& input = Input::getInput();

    switch (message) {
    // Window Notifications
    case WM_CLOSE:
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    // Keyboard Input Notifications
    case WM_KEYDOWN:
    case WM_KEYUP:
        input->onKeyboardInputNotifications(message, wparam, lparam);
        break;

    // Mouse Input Notifications
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        input->onMouseInputNotifications(message, wparam, lparam);
        break;

    // Raw Input Notifications
    /*case WM_INPUT:
        input->onRawInputNotifications(message, wparam, lparam);
        break;*/

    default: return DefWindowProc(windowHandle, message, wparam, lparam);
    }

    return 0;
}
