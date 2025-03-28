#pragma once
#include <Windows.h>
#include <windowsx.h>

#include <DirectXMath.h>

#include <vector>
#include <unordered_map>
#include <map>

#include <functional>
#include <future>

#include <string>

const int KEYS_QUANTITY = 256;
const int INPUT_TYPES_QUANTITY = 6; //20;

enum class InputType {
    NONE, LMBUTTON, RMBUTTON, MMBUTTON, MOUSE_MOVE, MOUSE_WHEEL,
};

enum class InputState {
    NONE, BEGIN, CHANGE, END,
};

struct InputEvent {
    UINT message;
    WPARAM wparam;
    LPARAM lparam;
};

struct InputObject {
    int keyCode;
    InputType inputType;

    InputState inputState;

    DirectX::XMINT3 position;
    DirectX::XMINT3 delta;
};

typedef std::function<void(std::string actionName, InputObject inputObject)> ActionFunction;

//typedef void(*inputFunction)(const char* id, InputObject inputObject);

class Input {
    bool initialized;
    bool released;

    static std::unique_ptr<Input> input;

    std::vector<InputEvent> inputEvents;

    std::unordered_map<int, InputObject> keysInputObjects;
    std::unordered_map<InputType, InputObject> inputTypesInputObjects;

    std::unordered_map<std::string, ActionFunction> actionsFunctions;

    std::multimap<int, std::string> keysActions;
    std::multimap<InputType, std::string> inputTypesActions;
    
public:
    Input();
    ~Input();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    static std::unique_ptr<Input>& getInput();

    bool isKeyDown(int keyCode);

    DirectX::XMINT3 getMousePosition();
    DirectX::XMINT3 getMouseDelta();

    void initialize();
    void release();

    void addInputEvent(InputEvent inputEvent);
    void onInputEvents();

    void onKeyboardInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);
    void onMouseInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);
    void onRawInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);

    void bindAction(std::string name, ActionFunction function, const int* keyCodes, size_t keyCodesQuantity, const InputType* inputTypes = NULL, size_t inputTypesQuantity = 0);
    void bindAction(std::string name, ActionFunction function, const InputType* inputTypes, size_t inputTypesQuantity, const int* keyCodes = NULL, size_t keyCodesQuantity = 0);
    void unbindAction(std::string name);

    std::function<void(float, float)> test;

private:
    void onAllWindowMessages(InputEvent inputEvent);

    void performActions(int keyCode);
    void performActions(InputType inputType);
};

