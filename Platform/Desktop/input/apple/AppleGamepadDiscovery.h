#pragma once

#include "AppleGamepad.h"
#include "AppleGamepadRouting.h"
#include "Platform/input/GamepadRegistry.h"
#import <IOKit/hid/IOHIDManager.h>
#include <array>
#include <memory>

class nsAppleGamepadsTestAccess;

// Both native sources publish into the platform's existing registry.
class nsAppleGamepadDiscovery {
public:
    explicit nsAppleGamepadDiscovery(nsGamepadRegistry<nsAppleGamepad>& devices);
    ~nsAppleGamepadDiscovery();
    void Init();
    void Release();
    void Update(bool active);
    const IGamepad* GetGamepad(int index);
    void QueueGlfwConnect(int id, const char* name, int axes, int buttons);
    void QueueGlfwDisconnect(int id);

private:
    friend class nsAppleGamepadsTestAccess;
    struct nsCallbackOwner { nsAppleGamepadDiscovery* owner; };
    struct nsController {
        int nativeId;
        GCController* controller;
        nsAppleGamepadControls controls;
    };
    struct nsChange {
        bool connected;
        GCController* controller = nil;
        int nativeId = 0;
        std::string name;
        std::array<char, 33> guid{};
        int axes = 0;
        int buttons = 0;
        nsAppleGamepadControls controls;
    };
    struct nsHidChange {
        uint64_t identity;
        bool connected;
        std::array<char, 33> guid{};
        std::string name;
        bool supported = false;
        bool synthetic = false;
        id handle = nil;
    };
    struct nsHidHandle {
        id device;
        uint64_t identity;
    };
    struct nsGlfwSlot {
        bool present = false;
        std::string name;
        std::array<char, 33> guid{};
        int axes = 0;
        int buttons = 0;
    };

    nsGamepadRegistry<nsAppleGamepad>& _devices;
    nsAppleGamepadRouting _routing;
    std::vector<nsController> _controllers;
    std::vector<nsChange> _changes;
    std::vector<nsHidChange> _hidChanges;
    std::vector<nsHidHandle> _hidHandles;
    std::array<nsGlfwSlot, 16> _glfwSlots;
    std::array<bool, 16> _pendingGlfwChanges{};
    std::shared_ptr<nsCallbackOwner> _callbackOwner;
    id _connectedObserver = nil;
    id _disconnectedObserver = nil;
    IOHIDManagerRef _hidManager = nullptr;
    int _nextNativeId = -1;
    bool _initialized = false;
    bool _nativeEnabled = false;
    bool _active = false;

    void QueueController(GCController* controller, bool connected);
    void QueueHidDevice(IOHIDDeviceRef device, bool connected);
    void RescanHidDevices();
    void PublishChanges();
    void ConnectController(GCController* controller, nsAppleGamepadControls&& controls, const char* name);
    void DisconnectController(GCController* controller);
    void PublishGlfwSlot(int id);
    void UpdateControllers(bool active);
    void UpdateGlfw(bool active);
};
