#include "Input.h"

std::unique_ptr<Input> Input::input;

Input::Input() : keysInputObjects(), inputTypesInputObjects(), actionsFunctions(), keysActions(), inputTypesActions() {
    initialized = false;
    released = false;
}

Input::~Input() {
    release();
}

bool Input::isInitialized() {
    return initialized;
}

void Input::setInitialized() {
    initialized = true;
    released = false;
}

bool Input::isReleased() {
    return released;
}

void Input::setReleased() {
    initialized = false;
    released = true;
}

bool Input::isKeyDown(int keyCode) {
    return keysInputObjects[keyCode].inputState == InputState::BEGIN;
}

DirectX::XMINT3 Input::getMousePosition() {
    return inputTypesInputObjects[InputType::MOUSE_MOVE].position;
}

DirectX::XMINT3 Input::getMouseDelta() {
    return inputTypesInputObjects[InputType::MOUSE_MOVE].delta;
}

std::unique_ptr<Input>& Input::getInput() {
    if (!input) {
        input = std::make_unique<Input>();
    }

    return input;
}

void Input::initialize() {
    if (isInitialized()) {
        release();
    }

    InputObject inputObject = {};
    inputObject.inputState = InputState::END;

    keysInputObjects.reserve(KEYS_QUANTITY);
    for (int keyCode = 0; keyCode < KEYS_QUANTITY; keyCode++) {
        inputObject.keyCode = keyCode;

        keysInputObjects[keyCode] = inputObject;
    }

    inputObject.keyCode = 0;

    inputTypesInputObjects.reserve(INPUT_TYPES_QUANTITY);
    for (int i = (int)InputType::NONE; i < INPUT_TYPES_QUANTITY; i++) {
        InputType inputType = (InputType)i;

        inputObject.inputType = inputType;

        inputTypesInputObjects[inputType] = inputObject;
    }

    inputObject.inputState = InputState::CHANGE;
    inputTypesInputObjects[InputType::MOUSE_MOVE] = inputObject;
    inputTypesInputObjects[InputType::MOUSE_WHEEL] = inputObject;

    setInitialized();
}

void Input::release() {
    if (isReleased()) {
        return;
    }

    inputTypesActions.clear();
    keysActions.clear();

    actionsFunctions.clear();

    inputTypesInputObjects.clear();
    keysInputObjects.clear();

    setReleased();
}

void Input::onKeyboardInputNotifications(UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_KEYDOWN: {
        int keyCode = (int)wparam;

        if (keysInputObjects[keyCode].inputState != InputState::BEGIN) {
            keysInputObjects[keyCode].inputState = InputState::BEGIN;

            performActions(keyCode);
        }
        break;
    }
    case WM_KEYUP: {
        int keyCode = (int)wparam;

        if (keysInputObjects[keyCode].inputState != InputState::END) {
            keysInputObjects[keyCode].inputState = InputState::END;

            performActions(keyCode);
        }
        break;
    }
    }
}

void Input::onMouseInputNotifications(UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_LBUTTONDOWN: {
        InputType inputType = InputType::LMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::BEGIN) {
            inputTypesInputObjects[inputType].inputState = InputState::BEGIN;

            performActions(inputType);
        }
        break;
    }
    case WM_LBUTTONUP: {
        InputType inputType = InputType::LMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::END) {
            inputTypesInputObjects[inputType].inputState = InputState::END;

            performActions(inputType);
        }
        break;
    }
    case WM_MBUTTONDOWN: {
        InputType inputType = InputType::MMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::BEGIN) {
            inputTypesInputObjects[inputType].inputState = InputState::BEGIN;

            performActions(inputType);
        }
        break;
    }
    case WM_MBUTTONUP: {
        InputType inputType = InputType::MMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::END) {
            inputTypesInputObjects[inputType].inputState = InputState::END;

            performActions(inputType);
        }
        break;
    }
    case WM_MOUSEMOVE: {
        InputType inputType = InputType::MOUSE_MOVE;

        InputObject inputObject = inputTypesInputObjects[inputType];

        DirectX::XMINT3 position = DirectX::XMINT3(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam), 0);
        DirectX::XMINT3 delta = DirectX::XMINT3(position.x - inputObject.position.x, position.y - inputObject.position.y, 0);

        inputObject.position = position;
        inputObject.delta = delta;

        inputTypesInputObjects[inputType] = inputObject;

        performActions(inputType);
        break;
    }
    case WM_MOUSEWHEEL: {
        InputType inputType = InputType::MOUSE_WHEEL;

        InputObject inputObject = inputTypesInputObjects[inputType];

        DirectX::XMINT3 position = DirectX::XMINT3(0, 0, GET_WHEEL_DELTA_WPARAM(wparam));
        DirectX::XMINT3 delta = DirectX::XMINT3(0, 0, position.z - inputObject.position.z);

        inputObject.position = position;
        inputObject.delta = delta;

        inputTypesInputObjects[inputType] = inputObject;

        performActions(inputType);
        break;
    }
    case WM_RBUTTONDOWN: {
        InputType inputType = InputType::RMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::BEGIN) {
            inputTypesInputObjects[inputType].inputState = InputState::BEGIN;

            performActions(inputType);
        }
        break;
    }
    case WM_RBUTTONUP: {
        InputType inputType = InputType::RMBUTTON;

        if (inputTypesInputObjects[inputType].inputState != InputState::END) {
            inputTypesInputObjects[inputType].inputState = InputState::END;

            performActions(inputType);
        }
        break;
    }
    }
}

void Input::onRawInputNotifications(UINT message, WPARAM wparam, LPARAM lparam) {
    /*switch (message) {
    case WM_INPUT: {
        UINT size = 0;
        GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

        BYTE* buffer = new BYTE[size];
        if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) != size) {
            delete[] buffer;
            break;
        }

        RAWINPUT* raw = (RAWINPUT*)buffer;
        if (raw->header.dwType == RIM_TYPEMOUSE) {
            float deltaX = raw->data.mouse.lLastX;
            float deltaY = raw->data.mouse.lLastY;

            std::async(std::launch::async, test, deltaX, deltaY);
        }

        delete[] buffer;
        break;
    }
    }*/
}

void Input::bindAction(std::string name, ActionFunction function, int* keyCodes, size_t keyCodesSize, InputType* inputTypes, size_t inputTypesSize) {
    if (keyCodesSize < 0 || inputTypesSize < 0) {
        return;
    }
    if (keyCodesSize == 0 && inputTypesSize == 0) {
        return;
    }
    if (keyCodesSize > 0 && !keyCodes) {
        return;
    }
    if (inputTypesSize > 0 && !inputTypes) {
        return;
    }
    
    actionsFunctions[name] = function;

    for (int i = 0; i < keyCodesSize; i++) {
        keysActions.insert(std::make_pair(keyCodes[i], name));
    }

    for (int i = 0; i < inputTypesSize; i++) {
        inputTypesActions.insert(std::make_pair(inputTypes[i], name));
    }
}

void Input::bindAction(std::string name, ActionFunction function, InputType* inputTypes, size_t inputTypesSize, int* keyCodes, size_t keyCodesSize) {
    bindAction(name, function, keyCodes, keyCodesSize, inputTypes, inputTypesSize);
}

void Input::unbindAction(std::string name) {
    actionsFunctions.erase(name);

    for (auto iterator = keysActions.begin(); iterator != keysActions.end();) {
        if (iterator->second == name) {
            keysActions.erase(iterator);
        }
        else {
            iterator++;
        }
    }

    for (auto iterator = inputTypesActions.begin(); iterator != inputTypesActions.end();) {
        if (iterator->second == name) {
            inputTypesActions.erase(iterator);
        }
        else {
            iterator++;
        }
    }
}

void Input::performActions(int keyCode) {
    auto range = keysActions.equal_range(keyCode);
    for (auto iterator = range.first; iterator != range.second; iterator++) {
        std::async(std::launch::async, actionsFunctions[iterator->second], iterator->second, keysInputObjects[keyCode]);
    }
}

void Input::performActions(InputType inputType) {
    auto range = inputTypesActions.equal_range(inputType);
    for (auto iterator = range.first; iterator != range.second; iterator++) {
        //actionsFunctions[iterator->second](iterator->second, inputTypesInputObjects[inputType]);

        std::async(std::launch::async, actionsFunctions[iterator->second], iterator->second, inputTypesInputObjects[inputType]);
    }
}
