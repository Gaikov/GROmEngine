#include "GlfwGamepads.h"

#include "GlfwGamepadLayout.h"
#include <array>

namespace {
nsGlfwGamepads* g_gamepads = nullptr;
}

class nsDesktopGamepad final : public IGamepad {
public:
    nsDesktopGamepad(int nativeId, uint64_t connectionId) :
        _nativeId(nativeId), _connectionId(connectionId) {
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
        GLFWgamepadstate state{};
        if (glfwGetGamepadState(_nativeId, &state)) Update(state);
        else ClearInput();
    }

    void Update(const GLFWgamepadstate& state) {
        for (auto i = 0; i < GetAxisCount(); ++i) _axes[i] = GlfwGamepadAxis(state, i);
        for (auto i = 0; i < GetButtonCount(); ++i) _buttons[i] = GlfwGamepadButton(state, i);
    }

    void ClearInput() {
        std::fill(_axes.begin(), _axes.end(), 0);
        std::fill(_buttons.begin(), _buttons.end(), 0);
    }

private:
    int _nativeId;
    uint64_t _connectionId;
    std::array<float, nsGamepadLayout::AxisCount> _axes{};
    std::array<uint8_t, nsGamepadLayout::ButtonCount> _buttons{};
};

nsGlfwGamepads::nsGlfwGamepads() = default;
nsGlfwGamepads::~nsGlfwGamepads() { Release(); }

void nsGlfwGamepads::Init() {
    if (_initialized) return;
    _initialized = true;
    g_gamepads = this;
    glfwSetJoystickCallback([](int id, int status) {
        if (!g_gamepads) return;
        if (status == GLFW_DISCONNECTED) g_gamepads->_devices.QueueDisconnect(id);
        else if (status == GLFW_CONNECTED) {
            if (glfwJoystickIsGamepad(id)) g_gamepads->_devices.QueueConnect(id,
                glfwGetJoystickName(id), nsGamepadLayout::AxisCount, nsGamepadLayout::ButtonCount);
        }
    });
}

void nsGlfwGamepads::Release() {
    if (g_gamepads == this) {
        glfwSetJoystickCallback(nullptr);
        g_gamepads = nullptr;
    }
    _initialized = false;
    _devices.Clear();
}

const IGamepad* nsGlfwGamepads::GetGamepad(int index) {
    const auto device = _devices.GetDevice(index);
    if (!device) return nullptr;
    if (!device->gamepad) {
        device->gamepad = std::make_unique<nsDesktopGamepad>(
            device->nativeId, device->connectionId);
    }
    return device->gamepad.get();
}

void nsGlfwGamepads::Update() {
    if (!_initialized) return;
    _devices.ApplyChanges();
    for (auto id = GLFW_JOYSTICK_1; id <= GLFW_JOYSTICK_LAST; ++id) {
        if (!glfwJoystickPresent(id)) {
            _devices.Disconnect(id);
            continue;
        }
        if (!glfwJoystickIsGamepad(id)) {
            _devices.Disconnect(id); // Raw devices require a platform mapping/decorator first.
            continue;
        }
        GLFWgamepadstate state{};
        if (!glfwGetGamepadState(id, &state)) {
            if (auto device = _devices.Find(id); device && device->gamepad) device->gamepad->ClearInput();
            continue;
        }
        auto& device = _devices.Connect(id, glfwGetJoystickName(id),
            nsGamepadLayout::AxisCount, nsGamepadLayout::ButtonCount);
        if (device.gamepad) device.gamepad->Update(state);
    }
}
