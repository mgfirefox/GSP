#pragma once
#include <Windows.h>
#include <windowsx.h>

#include <DirectXMath.h>

#include <unordered_map>
#include <map>

#include <functional>
#include <future>

#include <string>

//#define BS_THREAD_POOL_ENABLE
//#include <BS_thread_pool.hpp>
//#include "BS_thread_pool.hpp"

const int KEYS_QUANTITY = 256;
const int INPUT_TYPES_QUANTITY = 6; //20;

enum class InputType {
    NONE, LMBUTTON, RMBUTTON, MMBUTTON, MOUSE_MOVE, MOUSE_WHEEL,
};

enum class InputState {
    NONE, BEGIN, CHANGE, END,
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

    std::unordered_map<int, InputObject> keysInputObjects;
    std::unordered_map<InputType, InputObject> inputTypesInputObjects;

    std::unordered_map<std::string, ActionFunction> actionsFunctions;

    std::multimap<int, std::string> keysActions;
    std::multimap<InputType, std::string> inputTypesActions;

    static std::unique_ptr<Input> input;
    
public:
    Input();
    ~Input();

private:
    bool isInitialized();
    void setInitialized();

    bool isReleased();
    void setReleased();

public:
    bool isKeyDown(int keyCode);

    DirectX::XMINT3 getMousePosition();
    DirectX::XMINT3 getMouseDelta();

    static std::unique_ptr<Input>& getInput();

    void initialize();
    void release();

    void onKeyboardInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);
    void onMouseInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);
    void onRawInputNotifications(UINT message, WPARAM wparam, LPARAM lparam);

    void bindAction(std::string name, ActionFunction function, int* keyCodes, size_t keyCodesSize, InputType* inputTypes = NULL, size_t inputTypesSize = 0);
    void bindAction(std::string name, ActionFunction function, InputType* inputTypes, size_t inputTypesSize, int* keyCodes = NULL, size_t keyCodesSize = 0);
    void unbindAction(std::string name);

    std::function<void(float, float)> test;

private:
    void performActions(int keyCode);
    void performActions(InputType inputType);
};

