#include "GamepadMouse.h"
#include "IGamepads.h"
#include "MultiUserInput.h"
#include "Engine/console/Console.h"
#include "Engine/Platform.h"
#include "Engine/KeyCodes.h"
#include "Engine/EngineContext.h"
#include "Core/Config.h"
#include "Core/time/Time.h"
#include <algorithm>
#include <cmath>

namespace {
void CollectReceivers(IUserInput& input, std::vector<IUserInput*>& route) {
    route.push_back(&input);
    if (auto group = dynamic_cast<nsMultiUserInput*>(&input)) {
        for (auto child : group->GetInteractiveItems()) CollectReceivers(*child, route);
        route.push_back(nullptr);
    }
}

float Parameter(const nsVar* var, float fallback, float minimum, float maximum) {
    const auto value = var->Value();
    return std::isfinite(value) ? std::clamp(value, minimum, maximum) : fallback;
}

float Axis(float value) {
    return std::isfinite(value) ? std::clamp(value, -1.0f, 1.0f) : 0;
}
}

bool nsGamepadMouse::OnInit() {
    _sensitivity = g_cfg->RegVar("pad_ui_sensitivity", "2", GVF_SAVABLE);
    _deadzone = g_cfg->RegVar("pad_ui_deadzone", "0.05", GVF_SAVABLE);
    _exponent = g_cfg->RegVar("pad_ui_exponent", "0.5", GVF_SAVABLE);
    _escapeRoute.reserve(16);
    _escapeNextRoute.reserve(16);
    return true;
}

nsGamepadMouse::nsDevice* nsGamepadMouse::Find(uint64_t id) {
    for (auto& device : _devices) if (device.id == id) return &device;
    return nullptr;
}

bool nsGamepadMouse::Cancel(bool notify) {
    const auto generation = ++_generation;
    const auto held = _mouseDown;
    _mouseDown = false;
    _owner = 0;
    for (auto& device : _devices) {
        for (auto i = 0; i < 2; ++i) {
            device.awaitRelease[i] = true; // Wait for a neutral sample, even if the last sample was up.
            device.pressed[i] = false;
        }
    }
    if (notify && held) nsEngineContext::Shared()->CancelPointer(NS_POINTER_PRIMARY);
    return _generation == generation;
}

void nsGamepadMouse::SyncMode() {
    const auto active = enabled.IsLocked();
    if (_wasEnabled == active) return;
    _wasEnabled = active;
    Cancel();
    _running = _hasPosition = false;
    _devices.clear();
}

bool nsGamepadMouse::IsBlocked() const {
    auto platform = App_GetPlatform();
    return !_appActive || _physicalDown || nsConsole::Shared()->IsActive()
        || (platform->IsKeyPressed(NS_KEY_LEFT_ALT) && platform->IsKeyPressed(NS_KEY_TAB));
}

void nsGamepadMouse::OnPointerEvent(nsPointerEvent event, int id, int x, int y) {
    SyncMode();
    if (id == NS_POINTER_PRIMARY) {
        if (event == nsPointerEvent::Down) _physicalDown = true;
        else if (event == nsPointerEvent::Up) _physicalDown = false;
    }
    if (enabled.IsLocked()) {
        Cancel();
        _x = static_cast<float>(x);
        _y = static_cast<float>(y);
        _hasPosition = true;
    }
}

void nsGamepadMouse::OnPointerCancel(int id) {
    if (id != NS_POINTER_PRIMARY) return;
    _physicalDown = false;
    Cancel(false);
}

void nsGamepadMouse::OnActivateApp(bool active) {
    _appActive = active;
    if (!active) {
        _physicalDown = false;
        Cancel();
        _running = false;
        _devices.clear();
    }
}

bool nsGamepadMouse::GetCursorPos(float& x, float& y) {
    SyncMode();
    if (!_wasEnabled) return false;
    int width = 0, height = 0;
    App_GetPlatform()->GetClientSize(width, height);
    if (width > 0 && height > 0) SeedPosition(width, height);
    x = _hasPosition ? _x : 0;
    y = _hasPosition ? _y : 0;
    return true;
}

void nsGamepadMouse::SetCursorPos(float x, float y) {
    SyncMode();
    if (!_wasEnabled || !std::isfinite(x) || !std::isfinite(y)) return;
    int width = 0, height = 0;
    App_GetPlatform()->GetClientSize(width, height);
    _x = std::clamp(x, 0.0f, static_cast<float>(std::max(0, width - 1)));
    _y = std::clamp(y, 0.0f, static_cast<float>(std::max(0, height - 1)));
    _hasPosition = true;
}

void nsGamepadMouse::SendEscape() {
    auto context = nsEngineContext::Shared();
    auto ui = context->GetActiveInput();
    _escapeRoute.clear();
    CollectReceivers(*ui, _escapeRoute);
    const auto generation = _generation;
    ui->OnKeyDown(NS_KEY_ESCAPE, false, 0);
    ui = context->GetActiveInput();
    _escapeNextRoute.clear();
    CollectReceivers(*ui, _escapeNextRoute);
    if (_generation == generation && enabled.IsLocked() && !IsBlocked()
        && _escapeRoute == _escapeNextRoute) {
        ui->OnKeyUp(NS_KEY_ESCAPE, 0);
    }
}

void nsGamepadMouse::SeedPosition(int width, int height) {
    if (!_hasPosition) {
        int x = -1, y = -1;
        auto platform = App_GetPlatform();
        if (!platform->IsMobile()) platform->GetCursorPos(x, y);
        _x = x >= 0 && x < width ? static_cast<float>(x) : width * 0.5f;
        _y = y >= 0 && y < height ? static_cast<float>(y) : height * 0.5f;
        _hasPosition = true;
    }
    _x = std::clamp(_x, 0.0f, static_cast<float>(width - 1));
    _y = std::clamp(_y, 0.0f, static_cast<float>(height - 1));
}

void nsGamepadMouse::Update() {
    const auto now = nsTime::GetTimeUS();
    const auto seconds = _lastUpdate ? static_cast<float>((now - _lastUpdate) / 1000000.0) : 0;
    _lastUpdate = now;
    UpdateFrame(App_GetPlatform()->GetGamepadsProvider(), seconds);
}

void nsGamepadMouse::UpdateFrame(IGamepads* provider, float seconds) {
    SyncMode();
    if (!_wasEnabled || IsBlocked()) {
        if (_running) Cancel();
        _running = false;
        _devices.clear();
        return;
    }
    if (!_running) {
        _running = true;
        seconds = 0;
    }
    int width = 0, height = 0;
    App_GetPlatform()->GetClientSize(width, height);
    if (width <= 0 || height <= 0) { Cancel(); return; }
    const auto hadPosition = _hasPosition;
    const auto previousX = static_cast<int>(_x), previousY = static_cast<int>(_y);
    SeedPosition(width, height);
    const auto oldX = hadPosition ? previousX : static_cast<int>(_x);
    const auto oldY = hadPosition ? previousY : static_cast<int>(_y);
    seconds = std::isfinite(seconds) ? std::clamp(seconds, 0.0f, 0.1f) : 0;
    const auto deadzone = Parameter(_deadzone, 0.05f, 0, 0.95f);
    for (auto& device : _devices) device.seen = false;
    uint64_t candidate = 0;
    for (auto index = 0; provider && index < provider->GetCount(); ++index) {
        const auto pad = provider->GetGamepad(index);
        if (!pad) continue;
        auto device = Find(pad->GetConnectionId());
        if (!device) {
            _devices.push_back({pad->GetConnectionId()});
            device = &_devices.back();
            for (auto button = 0; button < 2; ++button) device->awaitRelease[button] = pad->IsButtonPressed(button);
        }
        device->seen = true;
        auto fresh = false;
        for (auto button = 0; button < 2; ++button) {
            const auto down = pad->IsButtonPressed(button);
            if (!down) device->awaitRelease[button] = false;
            device->pressed[button] = down && !device->buttons[button] && !device->awaitRelease[button];
            device->buttons[button] = down;
            fresh |= device->pressed[button];
        }
        device->x = Axis(pad->GetAxisValue(nsGamepadLayout::LeftX));
        device->y = Axis(pad->GetAxisValue(nsGamepadLayout::LeftY));
        const auto moving = std::hypot(device->x, device->y) > deadzone;
        fresh |= moving && !device->stickActive;
        device->stickActive = moving;
        if (fresh && !candidate) candidate = device->id;
    }
    if (auto owner = Find(_owner); _owner && (!owner || !owner->seen)) {
        Cancel();
        candidate = 0;
    }
    std::erase_if(_devices, [](const auto& device) { return !device.seen; });
    if (!_mouseDown && candidate) _owner = candidate;
    const auto owner = Find(_owner);
    if (!owner) return;
    // Copy only scalar input before dispatch: callbacks can reset the emulator.
    const auto x = owner->x, y = owner->y;
    const auto click = owner->buttons[0];
    const auto pressClick = owner->pressed[0], pressBack = owner->pressed[1];
    const auto generation = _generation;
    if (pressBack) {
        if (_mouseDown) {
            if (!Cancel() || !enabled.IsLocked() || IsBlocked()) return;
        }
        SendEscape();
        return;
    }
    const auto radius = std::hypot(x, y);
    if (radius > deadzone) {
        const auto amount = (std::min(radius, 1.0f) - deadzone) / (1.0f - deadzone);
        const auto speed = Parameter(_sensitivity, 2, 0, 10) * height
            * std::pow(amount, Parameter(_exponent, 0.5f, 0.1f, 8));
        _x = std::clamp(_x + x / radius * speed * seconds, 0.0f, static_cast<float>(width - 1));
        _y = std::clamp(_y + y / radius * speed * seconds, 0.0f, static_cast<float>(height - 1));
    }
    const auto clientX = static_cast<int>(_x), clientY = static_cast<int>(_y);
    if (oldX != clientX || oldY != clientY) {
        auto ui = nsEngineContext::Shared()->GetActiveInput();
        ui->OnPointerMove(static_cast<float>(clientX), static_cast<float>(clientY), NS_POINTER_PRIMARY);
        if (_generation != generation || !enabled.IsLocked() || IsBlocked()) return;
    }
    if (pressClick) {
        _mouseDown = true;
        auto ui = nsEngineContext::Shared()->GetActiveInput();
        ui->OnPointerDown(static_cast<float>(clientX), static_cast<float>(clientY), NS_POINTER_PRIMARY);
    } else if (_mouseDown && !click) {
        _mouseDown = false;
        auto ui = nsEngineContext::Shared()->GetActiveInput();
        ui->OnPointerUp(static_cast<float>(clientX), static_cast<float>(clientY), NS_POINTER_PRIMARY);
    }
}
