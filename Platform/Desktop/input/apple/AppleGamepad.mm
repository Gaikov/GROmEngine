#include "AppleGamepad.h"

#include <algorithm>
#include <cmath>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace {
float Normalize(float value) {
    return std::isfinite(value) ? std::clamp(value, -1.0f, 1.0f) : 0;
}

bool IsChildOf(GCControllerElement* element, GCControllerElement* collection) {
    if (!collection) return false;
    for (auto parent = element.collection; parent; parent = parent.collection) {
        if (parent == collection) return true;
    }
    return false;
}
}

nsAppleGamepadControls nsAppleGamepadControls::Discover(GCController* controller) {
    nsAppleGamepadControls result;
    if (@available(macOS 11.0, *)) {
        auto profile = controller.physicalInputProfile;
        result.profile = profile;
        result.extended = controller.extendedGamepad;
        result.nativeAxes = [profile.axes copy];
        result.nativeButtons = [profile.buttons copy];
        auto pad = result.extended;
        if (pad) {
            result.axes = {pad.leftThumbstick.xAxis, pad.leftThumbstick.yAxis,
                pad.rightThumbstick.xAxis, pad.rightThumbstick.yAxis};
            result.buttons = {pad.buttonA, pad.buttonB, pad.buttonX, pad.buttonY,
                pad.leftShoulder, pad.rightShoulder, pad.leftTrigger, pad.rightTrigger,
                pad.buttonOptions, pad.buttonMenu, pad.leftThumbstickButton, pad.rightThumbstickButton,
                pad.dpad.up, pad.dpad.down, pad.dpad.left, pad.dpad.right, pad.buttonHome};
            if (@available(macOS 12.0, *)) {
                if (auto share = result.nativeButtons[GCInputButtonShare]) result.buttons.push_back(share);
            }
        }
        for (NSString* alias in [[result.nativeAxes allKeys] sortedArrayUsingSelector:@selector(compare:)]) {
            auto input = result.nativeAxes[alias];
            if (std::find(result.axes.begin(), result.axes.end(), input) != result.axes.end()) continue;
            if (pad && (IsChildOf(input, pad.dpad) || IsChildOf(input, pad.leftThumbstick)
                || IsChildOf(input, pad.rightThumbstick))) continue;
            result.axes.push_back(input);
        }
        for (NSString* alias in [[result.nativeButtons allKeys] sortedArrayUsingSelector:@selector(compare:)]) {
            auto input = result.nativeButtons[alias];
            if (std::find(result.buttons.begin(), result.buttons.end(), input) != result.buttons.end()) continue;
            if (pad && (IsChildOf(input, pad.leftThumbstick) || IsChildOf(input, pad.rightThumbstick)
                || IsChildOf(input, pad.dpad))) continue;
            // Keep slot 17 reserved for Share even on a controller without it.
            if (pad && result.buttons.size() == 17) result.buttons.push_back(nil);
            result.buttons.push_back(input);
        }
    }
    return result;
}

bool nsAppleGamepadControls::Matches(GCController* controller) const {
    if (@available(macOS 11.0, *)) {
        auto current = controller.physicalInputProfile;
        if (current != profile || controller.extendedGamepad != extended) return false;
        auto axesNow = current.axes;
        auto buttonsNow = current.buttons;
        if (axesNow.count != nativeAxes.count || buttonsNow.count != nativeButtons.count) return false;
        for (NSString* alias in nativeAxes) if (axesNow[alias] != nativeAxes[alias]) return false;
        for (NSString* alias in nativeButtons) if (buttonsNow[alias] != nativeButtons[alias]) return false;
        return true;
    }
    return false;
}

nsAppleGamepad::nsAppleGamepad(uint64_t connectionId, GCController* controller,
    const nsAppleGamepadControls& controls, bool active) :
    _connectionId(connectionId), _controller(controller), _controls(controls),
    _axes(controls.axes.size()), _buttons(controls.buttons.size()),
    _awaitRelease(controls.buttons.size()) {
    Update(active);
}

nsAppleGamepad::nsAppleGamepad(uint64_t connectionId, int nativeId, int axes, int buttons, bool active) :
    _connectionId(connectionId), _nativeId(nativeId), _axes(axes), _buttons(buttons),
    _awaitRelease(buttons) {
    Update(active);
}

float nsAppleGamepad::GetAxisValue(int index) const {
    return index >= 0 && index < GetAxisCount() ? _axes[index] : 0;
}

bool nsAppleGamepad::IsButtonPressed(int index) const {
    return index >= 0 && index < GetButtonCount() && _buttons[index] != 0;
}

void nsAppleGamepad::Update(bool active) {
    if (!active) {
        std::fill(_awaitRelease.begin(), _awaitRelease.end(), 1);
        ClearInput();
        return;
    }
    if (_controller) {
        for (auto i = size_t(0); i < _axes.size(); ++i) _axes[i] = Normalize(_controls.axes[i].value);
        for (auto i = 0; i < GetButtonCount(); ++i) UpdateButton(i, _controls.buttons[i].isPressed);
    } else {
        int axes = 0, buttons = 0;
        auto axisValues = glfwGetJoystickAxes(_nativeId, &axes);
        auto buttonValues = glfwGetJoystickButtons(_nativeId, &buttons);
        if (axes != GetAxisCount() || buttons != GetButtonCount()
            || (axes && !axisValues) || (buttons && !buttonValues)) ClearInput();
        else Update(axisValues, buttonValues);
    }
}

void nsAppleGamepad::Update(const float* axes, const unsigned char* buttons) {
    for (auto i = 0; i < GetAxisCount(); ++i) _axes[i] = Normalize(axes[i]);
    for (auto i = 0; i < GetButtonCount(); ++i) UpdateButton(i, buttons[i] == GLFW_PRESS);
}

void nsAppleGamepad::UpdateButton(int index, bool down) {
    if (!down) _awaitRelease[index] = 0;
    _buttons[index] = down && !_awaitRelease[index];
}

void nsAppleGamepad::ClearInput() {
    std::fill(_axes.begin(), _axes.end(), 0);
    std::fill(_buttons.begin(), _buttons.end(), 0);
}
