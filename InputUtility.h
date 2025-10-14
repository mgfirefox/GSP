#pragma once

#include <DirectXMath.h>

#include <functional>

#include <string>

#include "IntUtility.h"

enum class InputType : uint8
{
    Undefined,

    LeftMouseButton,
    RightMouseButton,
    MiddleMouseButton,

    MouseMovement,
    MouseWheel,
};

enum class InputState : uint8
{
    Undefined,

    Begin,
    Change,
    End,
};

struct InputEvent
{
    UINT message;
    WPARAM wparam;
    LPARAM lparam;
};

struct InputVector3
{
    DirectX::XMINT3 xyz;

    int32 getX()
    {
        return xyz.x;
    }

    void setX(int32 x)
    {
        xyz.x = x;
    }

    int32 getY()
    {
        return xyz.y;
    }

    void setY(int32 y)
    {
        xyz.y = y;
    }

    int32 getZ()
    {
        return xyz.z;
    }

    void setZ(int32 z)
    {
        xyz.z = z;
    }

    DirectX::XMINT2 getXy()
    {
        return {xyz.x, xyz.y};
    }

    void setXy(DirectX::XMINT2 xy)
    {
        xyz.x = xy.x;
        xyz.y = xy.y;
    }

    void clear()
    {
        xyz = {};
    }
};

struct InputObject
{
    int32 keyCode;
    InputType inputType;

    InputState inputState;

    InputVector3 position;
    InputVector3 delta;
};

using ActionFunction = std::function<void(std::string actionName, InputObject inputObject)>;
