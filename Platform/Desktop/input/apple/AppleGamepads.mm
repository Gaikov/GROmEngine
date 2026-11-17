#include "AppleGamepadDiscovery.h"
#include "AppleGamepads.h"

#import <AppKit/AppKit.h>
#import <IOKit/hid/IOHIDKeys.h>
#import <IOKit/hid/IOHIDUsageTables.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdio>
#include <limits>
#include "../GlfwGamepadLayout.h"

static_assert(GLFW_JOYSTICK_LAST + 1 == 16);

namespace {
nsAppleGamepads* g_gamepads = nullptr;

uint32_t GetNumber(IOHIDDeviceRef device, CFStringRef key) {
    auto property = IOHIDDeviceGetProperty(device, key);
    int32_t value = 0;
    if (property && CFGetTypeID(property) == CFNumberGetTypeID()) {
        CFNumberGetValue(static_cast<CFNumberRef>(property), kCFNumberSInt32Type, &value);
    }
    return static_cast<uint32_t>(value);
}

std::array<char, 33> GlfwGuid(int nativeId) {
    std::array<char, 33> result{};
    if (auto guid = glfwGetJoystickGUID(nativeId)) std::snprintf(result.data(), result.size(), "%s", guid);
    return result;
}
}

nsAppleGamepads::nsAppleGamepads() = default;
nsAppleGamepads::~nsAppleGamepads() { Release(); }

void nsAppleGamepads::Init() {
    if (g_gamepads == this) return;
    if (!_discovery) _discovery = std::make_unique<nsAppleGamepadDiscovery>(_devices);
    _discovery->Init();
    g_gamepads = this;
    glfwSetJoystickCallback([](int id, int status) {
        if (!g_gamepads) return;
        if (status == GLFW_DISCONNECTED) g_gamepads->_discovery->QueueGlfwDisconnect(id);
        else if (status == GLFW_CONNECTED) {
            if (glfwJoystickIsGamepad(id)) g_gamepads->_discovery->QueueGlfwConnect(id,
                glfwGetJoystickName(id), nsGamepadLayout::AxisCount, nsGamepadLayout::ButtonCount);
        }
    });
}

void nsAppleGamepads::Release() {
    if (g_gamepads == this) {
        glfwSetJoystickCallback(nullptr);
        g_gamepads = nullptr;
    }
    _discovery.reset();
    _devices.Clear();
}

void nsAppleGamepads::Update() {
    if (_discovery) _discovery->Update(NSApp.isActive);
}

const IGamepad* nsAppleGamepads::GetGamepad(int index) {
    return _discovery ? _discovery->GetGamepad(index) : nullptr;
}

nsAppleGamepadDiscovery::nsAppleGamepadDiscovery(nsGamepadRegistry<nsAppleGamepad>& devices) : _devices(devices) {}
nsAppleGamepadDiscovery::~nsAppleGamepadDiscovery() { Release(); }

void nsAppleGamepadDiscovery::Init() {
    if (_initialized) return;
    _initialized = true;
    if (@available(macOS 11.0, *)) {
        _nativeEnabled = true;
        _callbackOwner = std::make_shared<nsCallbackOwner>(nsCallbackOwner{this});
        auto callbacks = _callbackOwner;
        auto center = NSNotificationCenter.defaultCenter;
        _connectedObserver = [center addObserverForName:GCControllerDidConnectNotification object:nil
            queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification* notification) {
                if (auto owner = callbacks->owner) owner->QueueController(notification.object, true);
            }];
        _disconnectedObserver = [center addObserverForName:GCControllerDidDisconnectNotification object:nil
            queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification* notification) {
                if (auto owner = callbacks->owner) owner->QueueController(notification.object, false);
            }];

        _hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
        if (_hidManager) {
            auto matching = @[
                @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_Joystick)},
                @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_GamePad)},
                @{@kIOHIDDeviceUsagePageKey: @(kHIDPage_GenericDesktop), @kIOHIDDeviceUsageKey: @(kHIDUsage_GD_MultiAxisController)}];
            IOHIDManagerSetDeviceMatchingMultiple(_hidManager, (__bridge CFArrayRef)matching);
            IOHIDManagerRegisterDeviceMatchingCallback(_hidManager,
                [](void* context, IOReturn, void*, IOHIDDeviceRef device) {
                    static_cast<nsAppleGamepadDiscovery*>(context)->QueueHidDevice(device, true);
                }, this);
            IOHIDManagerRegisterDeviceRemovalCallback(_hidManager,
                [](void* context, IOReturn, void*, IOHIDDeviceRef device) {
                    static_cast<nsAppleGamepadDiscovery*>(context)->QueueHidDevice(device, false);
                }, this);
            IOHIDManagerScheduleWithRunLoop(_hidManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
            if (IOHIDManagerOpen(_hidManager, kIOHIDOptionsTypeNone) != kIOReturnSuccess) {
                Log::Warning("Gamepad HID discovery unavailable; GLFW devices will wait for classification");
            }
            RescanHidDevices();
        } else Log::Warning("Could not create gamepad HID discovery");

        if (@available(macOS 11.3, *)) GCController.shouldMonitorBackgroundEvents = NO;
        // This also starts asynchronous discovery; later notifications complete it.
        for (GCController* controller in GCController.controllers) QueueController(controller, true);
    }
}

void nsAppleGamepadDiscovery::Release() {
    _initialized = false;
    if (_callbackOwner) _callbackOwner->owner = nullptr;
    auto center = NSNotificationCenter.defaultCenter;
    if (_connectedObserver) [center removeObserver:_connectedObserver];
    if (_disconnectedObserver) [center removeObserver:_disconnectedObserver];
    _connectedObserver = _disconnectedObserver = nil;
    _callbackOwner.reset();
    if (_hidManager) {
        IOHIDManagerRegisterDeviceMatchingCallback(_hidManager, nullptr, nullptr);
        IOHIDManagerRegisterDeviceRemovalCallback(_hidManager, nullptr, nullptr);
        IOHIDManagerUnscheduleFromRunLoop(_hidManager, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
        IOHIDManagerClose(_hidManager, kIOHIDOptionsTypeNone);
        CFRelease(_hidManager);
        _hidManager = nullptr;
    }
    _devices.Clear();
    _controllers.clear();
    _changes.clear();
    _hidChanges.clear();
    _hidHandles.clear();
    _routing.Clear();
    for (auto& slot : _glfwSlots) slot.present = false;
    _pendingGlfwChanges.fill(false);
    _nativeEnabled = _active = false;
}

void nsAppleGamepadDiscovery::QueueController(GCController* controller, bool connected) {
    if (!_initialized || !controller) return;
    nsChange change{connected, controller};
    if (connected) {
        // A disconnect before the next frame must not erase connect metadata.
        change.controls = nsAppleGamepadControls::Discover(controller);
        auto name = controller.vendorName.UTF8String;
        change.name = name ? name : "Unknown gamepad";
    }
    _changes.push_back(std::move(change));
}

void nsAppleGamepadDiscovery::QueueGlfwConnect(int id, const char* name, int axes, int buttons) {
    if (!_initialized || id < 0 || id >= static_cast<int>(_glfwSlots.size())) return;
    _changes.push_back({true, nil, id, name ? name : "Unknown gamepad", GlfwGuid(id), axes, buttons});
}

void nsAppleGamepadDiscovery::QueueGlfwDisconnect(int id) {
    if (_initialized && id >= 0 && id < static_cast<int>(_glfwSlots.size())) {
        _changes.push_back({false, nil, id});
    }
}

void nsAppleGamepadDiscovery::QueueHidDevice(IOHIDDeviceRef device, bool connected) {
    if (!_initialized) return;
    id handle = (__bridge id)device;
    uint64_t identity = 0;
    // Removal may arrive after the IOService has terminated. Use the retained
    // handle's saved identity instead of requiring another registry query.
    for (auto it = _hidChanges.rbegin(); it != _hidChanges.rend(); ++it) {
        if (it->handle == handle) { identity = it->identity; break; }
    }
    if (!identity) for (const auto& saved : _hidHandles) {
        if (saved.device == handle) { identity = saved.identity; break; }
    }
    if (!identity && IORegistryEntryGetRegistryEntryID(IOHIDDeviceGetService(device), &identity) != KERN_SUCCESS) return;
    if (!connected) {
        _hidChanges.push_back({identity, false, {}, {}, false, false, handle});
        return;
    }
    auto known = _routing.Contains(identity);
    for (auto it = _hidChanges.rbegin(); it != _hidChanges.rend(); ++it) {
        if (it->identity == identity) { known = it->connected; break; }
    }
    if (known) return;
    char name[256] = "Unknown";
    auto property = IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));
    if (property && CFGetTypeID(property) == CFStringGetTypeID()) {
        CFStringGetCString(static_cast<CFStringRef>(property), name, sizeof(name), kCFStringEncodingUTF8);
    }
    const auto guid = AppleGamepadGuid(GetNumber(device, CFSTR(kIOHIDVendorIDKey)),
        GetNumber(device, CFSTR(kIOHIDProductIDKey)), GetNumber(device, CFSTR(kIOHIDVersionNumberKey)), name);
    auto synthetic = IOHIDDeviceGetProperty(device, CFSTR("GCSyntheticDevice")) == kCFBooleanTrue;
    auto supported = false;
    if (@available(macOS 11.0, *)) supported = !synthetic && [GCController supportsHIDDevice:device];
    _hidChanges.push_back({identity, true, guid, name, supported, synthetic, handle});
}

void nsAppleGamepadDiscovery::RescanHidDevices() {
    if (!_hidManager) return;
    // Retry on focus gain too; a failed initial HID open must not be permanent.
    if (IOHIDManagerOpen(_hidManager, kIOHIDOptionsTypeNone) != kIOReturnSuccess) return;
    auto devices = IOHIDManagerCopyDevices(_hidManager);
    if (!devices) return;
    CFSetApplyFunction(devices, [](const void* device, void* context) {
        static_cast<nsAppleGamepadDiscovery*>(context)->QueueHidDevice(
            static_cast<IOHIDDeviceRef>(const_cast<void*>(device)), true);
    }, this);
    CFRelease(devices);
}

void nsAppleGamepadDiscovery::ConnectController(GCController* controller,
    nsAppleGamepadControls&& controls, const char* name) {
    for (const auto& source : _controllers) if (source.controller == controller) return;
    if (_nextNativeId == std::numeric_limits<int>::min()) return;
    controller.handlerQueue = dispatch_get_main_queue();
    auto nativeId = _nextNativeId--;
    _controllers.push_back({nativeId, controller, std::move(controls)});
    const auto& source = _controllers.back();
    // Keep the native identity even if the framework has not supplied its
    // profile yet. UpdateControllers publishes it once capabilities are ready.
    if (source.controls.profile) _devices.Connect(nativeId, name,
        static_cast<int>(source.controls.axes.size()), static_cast<int>(source.controls.buttons.size()));
}

void nsAppleGamepadDiscovery::DisconnectController(GCController* controller) {
    for (auto it = _controllers.begin(); it != _controllers.end(); ++it) {
        if (it->controller == controller) {
            _devices.Disconnect(it->nativeId);
            _controllers.erase(it);
            return;
        }
    }
}

void nsAppleGamepadDiscovery::PublishGlfwSlot(int id) {
    const auto& slot = _glfwSlots[id];
    const auto route = _nativeEnabled ? _routing.GetRoute(slot.guid.data(), slot.name.c_str())
        : nsAppleGamepadRouting::Route::Glfw;
    if (route == nsAppleGamepadRouting::Route::GameController) _devices.Disconnect(id);
    else if (route == nsAppleGamepadRouting::Route::Glfw) {
        _devices.Connect(id, slot.name.c_str(), slot.axes, slot.buttons);
    } else if (auto device = _devices.Find(id); device && device->gamepad) device->gamepad->ClearInput();
}

void nsAppleGamepadDiscovery::PublishChanges() {
    // Keep classification of a short-lived HID until its queued GLFW events
    // have been published. Neither callback ordering nor a later scan can
    // produce a second, synthetic connection.
    for (const auto& change : _hidChanges) {
        if (change.connected) {
            _routing.Connect(change.identity, change.guid.data(), change.name.c_str(), change.supported, change.synthetic);
            if (change.handle && std::none_of(_hidHandles.begin(), _hidHandles.end(), [&](const auto& saved) {
                return saved.identity == change.identity;
            })) _hidHandles.push_back({change.handle, change.identity});
        }
    }
    _pendingGlfwChanges.fill(false);
    auto pendingCount = size_t(0);
    for (auto index = size_t(0); index < _changes.size(); ++index) {
        auto& change = _changes[index];
        if (change.controller) {
            if (change.connected) ConnectController(change.controller, std::move(change.controls), change.name.c_str());
            else DisconnectController(change.controller);
        } else {
            // Retain an unclassified connect and everything following it in
            // this slot, including disconnects received in later frames.
            if (_pendingGlfwChanges[change.nativeId] || (change.connected && _nativeEnabled
                && _routing.GetRoute(change.guid.data(), change.name.c_str()) == nsAppleGamepadRouting::Route::Pending)) {
                _pendingGlfwChanges[change.nativeId] = true;
                if (pendingCount != index) _changes[pendingCount] = std::move(change);
                ++pendingCount;
                continue;
            }
            auto& slot = _glfwSlots[change.nativeId];
            slot.present = change.connected;
            if (change.connected) {
                slot.name = change.name;
                slot.guid = change.guid;
                slot.axes = change.axes;
                slot.buttons = change.buttons;
                PublishGlfwSlot(change.nativeId);
            } else _devices.Disconnect(change.nativeId);
        }
    }
    _changes.resize(pendingCount);
    for (const auto& change : _hidChanges) if (!change.connected) {
        auto latest = std::find_if(_hidChanges.rbegin(), _hidChanges.rend(), [&](const auto& next) {
            return next.identity == change.identity;
        });
        if (latest->connected) continue;
        _routing.Disconnect(change.identity);
        std::erase_if(_hidHandles, [&](const auto& saved) { return saved.identity == change.identity; });
    }
    _hidChanges.clear();
}

void nsAppleGamepadDiscovery::UpdateControllers(bool active) {
    for (auto& source : _controllers) {
        auto device = _devices.Find(source.nativeId);
        if (@available(macOS 11.0, *)) {
            if (!source.controller.physicalInputProfile) {
                if (device && device->gamepad) {
                    if (active) device->gamepad->ClearInput();
                    else device->gamepad->Update(false);
                }
                continue;
            }
        }
        if (!source.controls.Matches(source.controller)) {
            if (_nextNativeId == std::numeric_limits<int>::min()) continue;
            auto controls = nsAppleGamepadControls::Discover(source.controller);
            _devices.Disconnect(source.nativeId);
            source.nativeId = _nextNativeId--;
            source.controls = std::move(controls);
            device = nullptr;
            if (source.controls.profile) {
                device = &_devices.Connect(source.nativeId, source.controller.vendorName.UTF8String,
                    static_cast<int>(source.controls.axes.size()), static_cast<int>(source.controls.buttons.size()));
            }
        }
        if (device && device->gamepad) device->gamepad->Update(active);
    }
}

void nsAppleGamepadDiscovery::UpdateGlfw(bool active) {
    for (auto id = GLFW_JOYSTICK_1; id <= GLFW_JOYSTICK_LAST; ++id) {
        auto& slot = _glfwSlots[id];
        // A scan must not overtake queued connections in the same native slot.
        if (_pendingGlfwChanges[id]) {
            if (auto device = _devices.Find(id); device && device->gamepad) {
                if (active) device->gamepad->ClearInput();
                else device->gamepad->Update(false);
            }
            continue;
        }
        if (!glfwJoystickPresent(id)) {
            slot.present = false;
            _devices.Disconnect(id);
            continue;
        }
        if (!glfwJoystickIsGamepad(id)) {
            slot.present = false;
            _devices.Disconnect(id);
            continue;
        }
        constexpr auto axes = nsGamepadLayout::AxisCount;
        constexpr auto buttons = nsGamepadLayout::ButtonCount;
        GLFWgamepadstate state{};
        auto device = _devices.Find(id);
        if (!glfwGetGamepadState(id, &state)) {
            if (device && device->gamepad) {
                if (active) device->gamepad->ClearInput();
                else device->gamepad->Update(false);
            }
            continue;
        }
        if (!slot.present || slot.axes != axes || slot.buttons != buttons) {
            slot.present = true;
            auto name = glfwGetJoystickName(id);
            slot.name = name ? name : "Unknown gamepad";
            slot.guid = GlfwGuid(id);
            slot.axes = axes;
            slot.buttons = buttons;
        }
        PublishGlfwSlot(id);
        device = _devices.Find(id);
        if (device && device->gamepad) {
            if (!active) device->gamepad->Update(false);
            else if (!_nativeEnabled || _routing.GetRoute(slot.guid.data(), slot.name.c_str())
                == nsAppleGamepadRouting::Route::Glfw) device->gamepad->Update(state);
            else device->gamepad->ClearInput();
        }
    }
}

void nsAppleGamepadDiscovery::Update(bool active) {
    if (active && !_active) RescanHidDevices();
    _active = active;
    PublishChanges();
    UpdateControllers(active);
    UpdateGlfw(active);
}

const IGamepad* nsAppleGamepadDiscovery::GetGamepad(int index) {
    auto device = _devices.GetDevice(index);
    if (!device) return nullptr;
    if (!device->gamepad) {
        if (device->nativeId < 0) {
            for (const auto& source : _controllers) {
                if (source.nativeId == device->nativeId) {
                    device->gamepad = std::make_unique<nsAppleGamepad>(device->connectionId,
                        source.controller, source.controls, _active);
                    break;
                }
            }
        } else device->gamepad = std::make_unique<nsAppleGamepad>(device->connectionId, device->nativeId, _active);
    }
    return device->gamepad.get();
}
