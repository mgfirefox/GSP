#include "Input.h"

const int32 Input::keyCodeCount = 256;
const uint8 Input::typeCount = 5;

Input::Input() : inputEvents(), keyInputObjects(), typeInputObjects(), actionFunctions(),
                 keyActions(), typeActions()
{
    initialized = false;
    released = false;
}

Input::~Input()
{
    release();
}

bool Input::isInitialized()
{
    return initialized;
}

void Input::setInitialized()
{
    initialized = true;
    released = false;
}

bool Input::isReleased()
{
    return released;
}

void Input::setReleased()
{
    initialized = false;
    released = true;
}

bool Input::isKeyDown(int32 code)
{
    return keyInputObjects[code].inputState == InputState::Begin;
}

InputVector3 Input::getMousePosition()
{
    return typeInputObjects[InputType::MouseMovement].position;
}

InputVector3 Input::getMouseDelta()
{
    return typeInputObjects[InputType::MouseMovement].delta;
}

void Input::initialize()
{
    if (isInitialized())
    {
        release();
    }

    InputObject inputObject = {};
    inputObject.inputState = InputState::End;

    keyInputObjects.reserve(keyCodeCount);
    for (int32 code = 0; code < keyCodeCount; code++)
    {
        inputObject.keyCode = code;

        keyInputObjects[code] = inputObject;
    }

    inputObject.keyCode = 0;

    typeInputObjects.reserve(typeCount);
    for (uint8 i = 1; i <= typeCount; i++)
    {
        auto type = static_cast<InputType>(i);

        inputObject.inputType = type;

        typeInputObjects[type] = inputObject;
    }

    inputObject.inputState = InputState::Change;

    inputObject.position.clear();
    inputObject.position.setXy(DirectX::XMINT2(-1, -1));
    typeInputObjects[InputType::MouseMovement] = inputObject;

    inputObject.position.clear();
    inputObject.position.setZ(-1);
    typeInputObjects[InputType::MouseWheel] = inputObject;

    setInitialized();
}

void Input::release()
{
    if (isReleased())
    {
        return;
    }

    typeActions.clear();
    keyActions.clear();

    actionFunctions.clear();

    typeInputObjects.clear();
    keyInputObjects.clear();

    inputEvents.clear();

    setReleased();
}

void Input::addInputEvent(InputEvent inputEvent)
{
    inputEvents.push_back(inputEvent);
}

void Input::onInputEvents()
{
    clearInputTypeDelta();

    for (const auto& inputEvent : inputEvents)
    {
        onAllWindowMessages(inputEvent);
    }
    inputEvents.clear();
}

void Input::bindAction(std::string name, ActionFunction function, const int32* keyCodes,
                       uint64 keyCodeCount, const InputType* inputTypes,
                       uint64 inputTypeCount)
{
    if (keyCodeCount < 0 || inputTypeCount < 0)
    {
        return;
    }
    if (keyCodeCount == 0 && inputTypeCount == 0)
    {
        return;
    }
    if (keyCodeCount > 0 && !keyCodes)
    {
        return;
    }
    if (inputTypeCount > 0 && !inputTypes)
    {
        return;
    }

    actionFunctions[name] = function;

    for (int32 i = 0; i < keyCodeCount; i++)
    {
        keyActions.insert(std::make_pair(keyCodes[i], name));
    }

    for (int32 i = 0; i < inputTypeCount; i++)
    {
        typeActions.insert(std::make_pair(inputTypes[i], name));
    }
}

void Input::bindAction(std::string name, ActionFunction function, const InputType* inputTypes,
                       uint64 inputTypeCount, const int32* keyCodes,
                       uint64 keyCodeCount)
{
    bindAction(name, function, keyCodes, keyCodeCount, inputTypes, inputTypeCount);
}

void Input::unbindAction(std::string name)
{
    actionFunctions.erase(name);

    for (auto iterator = keyActions.begin(); iterator != keyActions.end();)
    {
        if (iterator->second == name)
        {
            iterator = keyActions.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }

    for (auto iterator = typeActions.begin(); iterator != typeActions.end();)
    {
        if (iterator->second == name)
        {
            iterator = typeActions.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }
}

void Input::clearInputTypeDelta()
{
    InputObject* inputObject = &typeInputObjects[InputType::MouseMovement];
    inputObject->delta.clear();

    inputObject = &typeInputObjects[InputType::MouseWheel];
    inputObject->delta.clear();
}

void Input::onAllWindowMessages(InputEvent inputEvent)
{
    switch (inputEvent.message)
    {
    case WM_KEYDOWN:
        {
            int32 keyCode = static_cast<int32>(inputEvent.wparam);

            /*wchar_t c[32];
            swprintf_s(c, 32, L"%d", keyCode);
            c[31] = '\0';
            OutputDebugString(L"\nKey Pressed: ");
            OutputDebugString(c);*/

            InputState& inputState = keyInputObjects[keyCode].inputState;
            if (inputState != InputState::Begin)
            {
                inputState = InputState::Begin;

                performActions(keyCode);
            }

            break;
        }
    case WM_KEYUP:
        {
            int32 keyCode = static_cast<int32>(inputEvent.wparam);

            /*wchar_t c[32];
            swprintf_s(c, 32, L"%d", keyCode);
            c[31] = '\0';
            OutputDebugString(L"\nKey Released: ");
            OutputDebugString(c);*/

            InputState& inputState = keyInputObjects[keyCode].inputState;
            if (inputState != InputState::End)
            {
                inputState = InputState::End;

                performActions(keyCode);
            }

            break;
        }

    case WM_LBUTTONDOWN:
        {
            auto inputType = InputType::LeftMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::Begin)
            {
                inputState = InputState::Begin;

                performActions(inputType);
            }

            break;
        }
    case WM_LBUTTONUP:
        {
            auto inputType = InputType::LeftMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::End)
            {
                inputState = InputState::End;

                performActions(inputType);
            }

            break;
        }
    case WM_MBUTTONDOWN:
        {
            auto inputType = InputType::MiddleMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::Begin)
            {
                inputState = InputState::Begin;

                performActions(inputType);
            }

            break;
        }
    case WM_MBUTTONUP:
        {
            auto inputType = InputType::MiddleMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::End)
            {
                inputState = InputState::End;

                performActions(inputType);
            }

            break;
        }
    case WM_MOUSEMOVE:
        {
            auto inputType = InputType::MouseMovement;

            InputObject& inputObject = typeInputObjects[inputType];

            DirectX::XMINT2 mousePosition = inputObject.position.getXy();
            DirectX::XMINT2 mouseDelta = inputObject.delta.getXy();

            if (mousePosition.x == -1 || mousePosition.y == -1)
            {
                auto position = DirectX::XMINT2(
                    GET_X_LPARAM(inputEvent.lparam), GET_Y_LPARAM(inputEvent.lparam));

                /*wchar_t c[32];
                swprintf_s(c, 32, L"%d %d", position.x, position.y);
                c[31] = '\0';
                OutputDebugString(L"\nInitial Mouse Position: ");
                OutputDebugString(c);*/

                inputObject.position.setXy(position);
            }
            else
            {
                auto position = DirectX::XMINT2(
                    GET_X_LPARAM(inputEvent.lparam), GET_Y_LPARAM(inputEvent.lparam));
                auto delta = DirectX::XMINT2(mouseDelta.x + (position.x - mousePosition.x),
                                             mouseDelta.y + (position.y - mousePosition.y));

                /*wchar_t c[32];
                swprintf_s(c, 32, L"%d %d", position.x, position.y);
                c[31] = '\0';
                OutputDebugString(L"\nMouse Position: ");
                OutputDebugString(c);*/

                /*wchar_t c[32];
                swprintf_s(c, 32, L"%d %d", delta.x, delta.y);
                c[31] = '\0';
                OutputDebugString(L"\nMouse Delta: ");
                OutputDebugString(c);*/

                inputObject.position.setXy(position);
                inputObject.delta.setXy(delta);
            }

            performActions(inputType);

            break;
        }
    case WM_MOUSEWHEEL:
        {
            auto inputType = InputType::MouseWheel;

            InputObject& inputObject = typeInputObjects[inputType];

            int32 wheelPosition = inputObject.position.getZ();
            int32 wheelDelta = inputObject.delta.getZ();

            if (wheelPosition == -1)
            {
                int32 position = GET_X_LPARAM(inputEvent.lparam);

                inputObject.position.setZ(position);
            }
            else
            {
                int32 position = GET_WHEEL_DELTA_WPARAM(inputEvent.wparam);
                int32 delta = wheelDelta + (position - wheelPosition);

                inputObject.position.setZ(position);
                inputObject.delta.setZ(delta);
            }

            performActions(inputType);

            break;
        }
    case WM_RBUTTONDOWN:
        {
            auto inputType = InputType::RightMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::Begin)
            {
                inputState = InputState::Begin;

                performActions(inputType);
            }

            break;
        }
    case WM_RBUTTONUP:
        {
            auto inputType = InputType::RightMouseButton;

            InputState& inputState = typeInputObjects[inputType].inputState;
            if (inputState != InputState::End)
            {
                inputState = InputState::End;

                performActions(inputType);
            }

            break;
        }
    default: ;
    }
}

void Input::performActions(int32 keyCode)
{
    auto range = keyActions.equal_range(keyCode);
    for (auto& iterator = range.first; iterator != range.second; ++iterator)
    {
        actionFunctions[iterator->second](iterator->second, keyInputObjects[keyCode]);
    }
}

void Input::performActions(InputType inputType)
{
    auto range = typeActions.equal_range(inputType);
    for (auto& iterator = range.first; iterator != range.second; ++iterator)
    {
        actionFunctions[iterator->second](iterator->second, typeInputObjects[inputType]);
    }
}
