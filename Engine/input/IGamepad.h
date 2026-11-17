#pragma once

#include <cstdint>
#include "GamepadLayout.h"

// Platform-owned, read-only snapshot. Reacquire after IGamepads::Update(): a
// disconnected device is destroyed there, while surviving devices stay put.
class IGamepad {
public:
    virtual ~IGamepad() = default;

    virtual uint64_t GetConnectionId() const = 0;
    // Xbox/web ordering: sticks 0..3, +X right, +Y down. Basic buttons 0..16.
    // Missing controls are neutral; extra controls follow the basic slots.
    // Values are finite [-1, 1], without a shared deadzone.
    // Counts stay fixed for this connection. Invalid indices read as neutral.
    virtual int GetAxisCount() const = 0;
    virtual float GetAxisValue(int axisIndex) const = 0;
    virtual int GetButtonCount() const = 0;
    virtual bool IsButtonPressed(int buttonIndex) const = 0;
};
