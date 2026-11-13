#include "DesktopGamepads.h"

#include <cmath>
#include <GLFW/glfw3.h>

class nsDesktopGamepad final : public IGamepad {
public:
    nsDesktopGamepad(int nativeId, uint64_t connectionId, int axes, int buttons) :
        _nativeId(nativeId), _connectionId(connectionId), _axes(axes), _buttons(buttons) {
        Update();
    }

    uint64_t GetConnectionId() const override { return _connectionId; }
    int GetAxisCount() const override { return static_cast<int>(_axes.size()); }
    int GetButtonCount() const override { return static_cast<int>(_buttons.size()); }
    float GetAxisValue(int index) const override {
        return index >= 0 && index < GetAxisCount() ? _axes[index] : 0;
    }
    bool IsButtonPressed(int index) const override {
        return index >= 0 && index < GetButtonCount() && _buttons[index] != 0;
    }

    void Update() {
        int axes = 0, buttons = 0;
        const auto axisValues = glfwGetJoystickAxes(_nativeId, &axes);
        const auto buttonValues = glfwGetJoystickButtons(_nativeId, &buttons);
        if (axes != GetAxisCount() || buttons != GetButtonCount()
            || (axes && !axisValues) || (buttons && !buttonValues)) ClearInput();
        else Update(axisValues, buttonValues);
    }

    void Update(const float* axes, const unsigned char* buttons) {
        for (auto i = 0; i < GetAxisCount(); ++i) {
            _axes[i] = std::isfinite(axes[i]) ? std::clamp(axes[i], -1.0f, 1.0f) : 0;
        }
        for (auto i = 0; i < GetButtonCount(); ++i) _buttons[i] = buttons[i] == GLFW_PRESS;
    }

    void ClearInput() {
        std::fill(_axes.begin(), _axes.end(), 0);
        std::fill(_buttons.begin(), _buttons.end(), 0);
    }

private:
    int _nativeId;
    uint64_t _connectionId;
    std::vector<float> _axes;
    std::vector<uint8_t> _buttons;
};

nsDesktopGamepads::nsDesktopGamepads() = default;
nsDesktopGamepads::~nsDesktopGamepads() = default;
void nsDesktopGamepads::Init() {}

void nsDesktopGamepads::Release() {
    _devices.Clear();
}

const IGamepad* nsDesktopGamepads::GetGamepad(int index) {
    const auto device = _devices.GetDevice(index);
    if (!device) return nullptr;
    if (!device->gamepad) {
        device->gamepad = std::make_unique<nsDesktopGamepad>(
            device->nativeId, device->connectionId, device->axisCount, device->buttonCount);
    }
    return device->gamepad.get();
}

void nsDesktopGamepads::Update() {
    _devices.ApplyChanges();
    for (auto id = GLFW_JOYSTICK_1; id <= GLFW_JOYSTICK_LAST; ++id) {
        if (!glfwJoystickPresent(id)) {
            _devices.Disconnect(id);
            continue;
        }
        int axes = 0, buttons = 0;
        const auto axisValues = glfwGetJoystickAxes(id, &axes);
        const auto buttonValues = glfwGetJoystickButtons(id, &buttons);
        const auto existing = _devices.Find(id);
        if ((axes && !axisValues) || (buttons && !buttonValues)
            || (existing && !axisValues && !buttonValues
                && (existing->axisCount || existing->buttonCount))) {
            if (existing && existing->gamepad) existing->gamepad->ClearInput();
            continue;
        }
        auto& device = _devices.Connect(id, glfwGetJoystickName(id), axes, buttons);
        if (device.gamepad) device.gamepad->Update(axisValues, buttonValues);
    }
}
