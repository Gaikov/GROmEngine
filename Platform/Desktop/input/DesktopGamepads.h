#pragma once

#include "Engine/input/IGamepad.h"
#include "Platform/input/GamepadRegistry.h"

#ifdef WEB_ASM
class nsWebGamepad;
using nsPlatformGamepad = nsWebGamepad;
#else
class nsDesktopGamepad;
using nsPlatformGamepad = nsDesktopGamepad;
#endif

// Implemented by the native desktop or web translation unit selected by CMake.
class nsDesktopGamepads {
public:
    nsDesktopGamepads();
    ~nsDesktopGamepads();
    void Init();
    void Release();
    void Update();
    void QueueConnect(int id, const char* name, int axes, int buttons) {
        _devices.QueueConnect(id, name, axes, buttons);
    }
    void QueueDisconnect(int nativeId) { _devices.QueueDisconnect(nativeId); }
    int GetCount() const { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index);

private:
    nsGamepadRegistry<nsPlatformGamepad> _devices;
#ifdef WEB_ASM
    bool _sampleAvailable = false;
#endif
};
