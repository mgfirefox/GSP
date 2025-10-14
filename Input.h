#pragma once
#include <Windows.h>
#include <windowsx.h>

#include <vector>
#include <unordered_map>
#include <map>

#include "IntUtility.h"

#include "InputUtility.h"

class Input
{
    bool initialized;
    bool released;

    static const int32 keyCodeCount;
    static const uint8 typeCount;

    std::vector<InputEvent> inputEvents;

    std::unordered_map<int32, InputObject> keyInputObjects;
    std::unordered_map<InputType, InputObject> typeInputObjects;

    std::unordered_map<std::string, ActionFunction> actionFunctions;

    std::multimap<int32, std::string> keyActions;
    std::multimap<InputType, std::string> typeActions;

public:
    Input();
    ~Input();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool isKeyDown(int32 code);

    InputVector3 getMousePosition();
    InputVector3 getMouseDelta();

    void initialize();
    void release();

    void addInputEvent(InputEvent inputEvent);
    void onInputEvents();

    void bindAction(std::string name, ActionFunction function, const int32* keyCodes,
                    uint64 keyCodeCount, const InputType* inputTypes = nullptr,
                    uint64 inputTypeCount = 0);
    void bindAction(std::string name, ActionFunction function, const InputType* inputTypes,
                    uint64 inputTypeCount, const int32* keyCodes = nullptr,
                    uint64 keyCodeCount = 0);
    void unbindAction(std::string name);

private:
    void clearInputTypeDelta();

    void onAllWindowMessages(InputEvent inputEvent);

    void performActions(int32 keyCode);
    void performActions(InputType inputType);
};
