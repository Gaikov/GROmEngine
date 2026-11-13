#pragma once

#include <cstdint>

// Platform-owned, read-only snapshot. Reacquire after UpdateGamepads(): a
// disconnected device is destroyed there, while surviving devices stay put.
class IGamepad {
public:
    virtual ~IGamepad() = default;

    virtual uint64_t GetConnectionId() const = 0;
    // Raw platform ordering, finite [-1, 1], without a shared deadzone.
    // Counts stay fixed for this connection. Invalid indices read as neutral.
    virtual int GetAxisCount() const = 0;
    virtual float GetAxisValue(int axisIndex) const = 0;
    virtual int GetButtonCount() const = 0;
    virtual bool IsButtonPressed(int buttonIndex) const = 0;
};
