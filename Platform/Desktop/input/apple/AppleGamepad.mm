#include "AppleGamepad.h"

#include <algorithm>
#include <cmath>
#include "../GlfwGamepadLayout.h"

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
        result.nativeAxes = [profile.axes copy];
        result.nativeButtons = [profile.buttons copy];
        if (!profile) return result;
        auto left = profile.dpads[GCInputLeftThumbstick];
        auto right = profile.dpads[GCInputRightThumbstick];
        auto dpad = profile.dpads[GCInputDirectionPad];
        result.axes = {left.xAxis, left.yAxis, right.xAxis, right.yAxis};
        result.buttons = {result.nativeButtons[GCInputButtonA], result.nativeButtons[GCInputButtonB],
            result.nativeButtons[GCInputButtonX], result.nativeButtons[GCInputButtonY],
            result.nativeButtons[GCInputLeftShoulder], result.nativeButtons[GCInputRightShoulder],
            result.nativeButtons[GCInputLeftTrigger], result.nativeButtons[GCInputRightTrigger],
            result.nativeButtons[GCInputButtonOptions], result.nativeButtons[GCInputButtonMenu],
            result.nativeButtons[GCInputLeftThumbstickButton], result.nativeButtons[GCInputRightThumbstickButton],
            dpad.up, dpad.down, dpad.left, dpad.right, result.nativeButtons[GCInputButtonHome]};
        if (@available(macOS 12.0, *)) {
            if (auto share = result.nativeButtons[GCInputButtonShare]) result.buttons.push_back(share);
        }
        for (NSString* alias in [[result.nativeAxes allKeys] sortedArrayUsingSelector:@selector(compare:)]) {
            auto input = result.nativeAxes[alias];
            if (std::find(result.axes.begin(), result.axes.end(), input) != result.axes.end()) continue;
            if (IsChildOf(input, dpad) || IsChildOf(input, left) || IsChildOf(input, right)) continue;
            result.axes.push_back(input);
        }
        for (NSString* alias in [[result.nativeButtons allKeys] sortedArrayUsingSelector:@selector(compare:)]) {
            auto input = result.nativeButtons[alias];
            if (std::find(result.buttons.begin(), result.buttons.end(), input) != result.buttons.end()) continue;
            if (IsChildOf(input, dpad) || IsChildOf(input, left) || IsChildOf(input, right)) continue;
            // Keep slot 17 reserved for Share even on a controller without it.
            if (result.buttons.size() == 17) result.buttons.push_back(nil);
            result.buttons.push_back(input);
        }
    }
    return result;
}

bool nsAppleGamepadControls::Matches(GCController* controller) const {
    if (@available(macOS 11.0, *)) {
        auto current = controller.physicalInputProfile;
        if (current != profile) return false;
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

nsAppleGamepad::nsAppleGamepad(uint64_t connectionId, int nativeId, bool active) :
    _connectionId(connectionId), _nativeId(nativeId), _axes(nsGamepadLayout::AxisCount),
    _buttons(nsGamepadLayout::ButtonCount), _awaitRelease(nsGamepadLayout::ButtonCount) {
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
        for (auto i = size_t(0); i < _axes.size(); ++i) {
            const auto value = Normalize(_controls.axes[i].value);
            _axes[i] = i == nsGamepadLayout::LeftY || i == nsGamepadLayout::RightY ? -value : value;
        }
        for (auto i = 0; i < GetButtonCount(); ++i) UpdateButton(i, _controls.buttons[i].isPressed);
    } else {
        GLFWgamepadstate state{};
        if (glfwGetGamepadState(_nativeId, &state)) Update(state);
        else ClearInput();
    }
}

void nsAppleGamepad::Update(const GLFWgamepadstate& state) {
    for (auto i = 0; i < GetAxisCount(); ++i) _axes[i] = GlfwGamepadAxis(state, i);
    for (auto i = 0; i < GetButtonCount(); ++i) UpdateButton(i, GlfwGamepadButton(state, i));
}

void nsAppleGamepad::UpdateButton(int index, bool down) {
    if (!down) _awaitRelease[index] = 0;
    _buttons[index] = down && !_awaitRelease[index];
}

void nsAppleGamepad::ClearInput() {
    std::fill(_axes.begin(), _axes.end(), 0);
    std::fill(_buttons.begin(), _buttons.end(), 0);
}
