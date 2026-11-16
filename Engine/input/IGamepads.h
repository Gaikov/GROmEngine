#pragma once

#include "IGamepad.h"

// Platform-owned provider. Consumers borrow it for the current platform lifecycle.
class IGamepads {
public:
    virtual ~IGamepads() = default;

    virtual void Init() = 0;
    virtual void Release() = 0;
    virtual void Update() = 0;

    virtual int GetCount() const = 0;
    virtual const IGamepad* GetGamepad(int index) = 0;
};
