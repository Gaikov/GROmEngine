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
    _deadzone = g_cfg->RegVar("pad_ui_deadzone", "0.02", GVF_SAVABLE);
    _exponent = g_cfg->RegVar("pad_ui_exponent", "2", GVF_SAVABLE);
    _escapeRoute.reserve(16);
    _escapeNextRoute.reserve(16);
    return true;
}

float nsGamepadMouse::GetAxisState(IGamepads* provider, int index) {
    auto sum = 0.0f;
    for (auto i = 0; provider && i < provider->GetCount(); ++i) {
        if (const auto pad = provider->GetGamepad(i)) {
            sum += Axis(pad->GetAxisValue(index));
        }
    }
    return std::clamp(sum, -1.0f, 1.0f);
}

bool nsGamepadMouse::IsButtonPressed(IGamepads* provider, int index) {
    for (auto i = 0; provider && i < provider->GetCount(); ++i) {
        const auto pad = provider->GetGamepad(i);
        if (pad && pad->IsButtonPressed(index)) return true;
    }
    return false;
}

bool nsGamepadMouse::Cancel(bool notify) {
    const auto generation = ++_generation;
    const auto held = _mouseDown;
    _mouseDown = false;
    _awaitRelease[0] = _awaitRelease[1] = true;
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
    const auto deadzone = Parameter(_deadzone, 0.02f, 0, 0.95f);
    for (auto& device : _devices) device.seen = false;
    for (auto index = 0; provider && index < provider->GetCount(); ++index) {
        const auto pad = provider->GetGamepad(index);
        if (!pad) continue;
        auto device = std::find_if(_devices.begin(), _devices.end(), [pad](const auto& item) {
            return item.id == pad->GetConnectionId();
        });
        if (device == _devices.end()) {
            _devices.push_back({pad->GetConnectionId()});
            device = _devices.end() - 1;
            for (auto button = 0; button < 2; ++button) {
                if (!_buttons[button] && pad->IsButtonPressed(button)) _awaitRelease[button] = true;
            }
        }
        device->seen = true;
        device->clickHeld = pad->IsButtonPressed(nsGamepadLayout::A);
    }
    const auto disconnectedClick = std::any_of(_devices.begin(), _devices.end(), [](const auto& device) {
        return !device.seen && device.clickHeld;
    });
    std::erase_if(_devices, [](const auto& device) { return !device.seen; });
    bool pressed[2]{};
    for (auto button = 0; button < 2; ++button) {
        const auto down = IsButtonPressed(provider, button);
        if (!down) _awaitRelease[button] = false;
        pressed[button] = down && !_buttons[button] && !_awaitRelease[button];
        _buttons[button] = down;
    }
    if (_mouseDown && disconnectedClick && !_buttons[0]) {
        Cancel();
        return;
    }
    const auto x = GetAxisState(provider, nsGamepadLayout::LeftX);
    const auto y = GetAxisState(provider, nsGamepadLayout::LeftY);
    // Copy only scalar input before dispatch: callbacks can reset the emulator.
    const auto click = _buttons[0];
    const auto pressClick = pressed[0], pressBack = pressed[1];
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
            * std::pow(amount, Parameter(_exponent, 2, 0.1f, 8));
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
