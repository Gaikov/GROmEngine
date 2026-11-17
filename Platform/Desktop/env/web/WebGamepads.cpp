#include "WebGamepads.h"

#include <cmath>
#include <cstring>
#include <emscripten/html5.h>

class nsWebGamepad final : public IGamepad {
public:
    nsWebGamepad(int nativeId, uint64_t connectionId, int axes, int buttons, bool sampleAvailable) :
        _connectionId(connectionId), _axes(axes), _buttons(buttons) {
        EmscriptenGamepadEvent sample{};
        if (sampleAvailable && emscripten_get_gamepad_status(nativeId, &sample) == EMSCRIPTEN_RESULT_SUCCESS) Update(sample);
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

    void Update(const EmscriptenGamepadEvent& sample) {
        if (!sample.connected
            || std::clamp(sample.numAxes, nsGamepadLayout::AxisCount, static_cast<int>(std::size(sample.axis))) != GetAxisCount()
            || std::clamp(sample.numButtons, nsGamepadLayout::ButtonCount, static_cast<int>(std::size(sample.digitalButton))) != GetButtonCount()) {
            ClearInput();
            return;
        }
        for (auto i = 0; i < GetAxisCount(); ++i) {
            const auto value = i < sample.numAxes ? sample.axis[i] : 0;
            _axes[i] = std::isfinite(value) ? static_cast<float>(std::clamp(value, -1.0, 1.0)) : 0;
        }
        for (auto i = 0; i < GetButtonCount(); ++i) _buttons[i] = i < sample.numButtons && sample.digitalButton[i];
    }

    void ClearInput() {
        std::fill(_axes.begin(), _axes.end(), 0);
        std::fill(_buttons.begin(), _buttons.end(), 0);
    }

private:
    uint64_t _connectionId;
    std::vector<float> _axes;
    std::vector<uint8_t> _buttons;
};

nsWebGamepads::nsWebGamepads() = default;
nsWebGamepads::~nsWebGamepads() { Release(); }

void nsWebGamepads::Init() {
    if (_initialized) return;
    _initialized = true;
    emscripten_set_gamepadconnected_callback(this, false,
        [](int, const EmscriptenGamepadEvent* event, void* data) -> EM_BOOL {
            if (std::strcmp(event->mapping, "standard") != 0) return false;
            static_cast<nsWebGamepads*>(data)->_devices.QueueConnect(event->index, event->id,
                std::clamp(event->numAxes, nsGamepadLayout::AxisCount, static_cast<int>(std::size(event->axis))),
                std::clamp(event->numButtons, nsGamepadLayout::ButtonCount, static_cast<int>(std::size(event->digitalButton))));
            return false;
        });
    emscripten_set_gamepaddisconnected_callback(this, false,
        [](int, const EmscriptenGamepadEvent* event, void* data) -> EM_BOOL {
            static_cast<nsWebGamepads*>(data)->_devices.QueueDisconnect(event->index);
            return false;
        });
}

void nsWebGamepads::Release() {
    if (_initialized) {
        emscripten_set_gamepadconnected_callback(nullptr, false, nullptr);
        emscripten_set_gamepaddisconnected_callback(nullptr, false, nullptr);
        _initialized = false;
    }
    _devices.Clear();
    _sampleAvailable = false;
}

const IGamepad* nsWebGamepads::GetGamepad(int index) {
    const auto device = _devices.GetDevice(index);
    if (!device) return nullptr;
    if (!device->gamepad) {
        device->gamepad = std::make_unique<nsWebGamepad>(
            device->nativeId, device->connectionId, device->axisCount, device->buttonCount, _sampleAvailable);
    }
    return device->gamepad.get();
}

void nsWebGamepads::Update() {
    if (!_initialized) return;
    _devices.ApplyChanges();
    const auto count = emscripten_sample_gamepad_data() == EMSCRIPTEN_RESULT_SUCCESS
        ? emscripten_get_num_gamepads() : -1;
    _sampleAvailable = count >= 0;
    if (!_sampleAvailable) {
        for (auto i = 0; i < _devices.GetCount(); ++i) {
            const auto device = _devices.GetDevice(i);
            if (device->gamepad) device->gamepad->ClearInput();
        }
        return;
    }
    for (auto i = _devices.GetCount() - 1; i >= 0; --i) {
        const auto id = _devices.GetDevice(i)->nativeId;
        if (id >= count) _devices.Disconnect(id);
    }
    for (auto id = 0; id < count; ++id) {
        EmscriptenGamepadEvent sample{};
        const auto result = emscripten_get_gamepad_status(id, &sample);
        if (result == EMSCRIPTEN_RESULT_NO_DATA || (result == EMSCRIPTEN_RESULT_SUCCESS && !sample.connected)) {
            _devices.Disconnect(id);
        } else if (result != EMSCRIPTEN_RESULT_SUCCESS) {
            const auto device = _devices.Find(id);
            if (device && device->gamepad) device->gamepad->ClearInput();
        } else if (std::strcmp(sample.mapping, "standard") != 0) {
            _devices.Disconnect(id); // No raw layout leaks into the engine.
        } else {
            const auto axes = std::clamp(sample.numAxes, nsGamepadLayout::AxisCount, static_cast<int>(std::size(sample.axis)));
            const auto buttons = std::clamp(sample.numButtons, nsGamepadLayout::ButtonCount, static_cast<int>(std::size(sample.digitalButton)));
            auto& device = _devices.Connect(id, sample.id, axes, buttons);
            if (device.gamepad) device.gamepad->Update(sample);
        }
    }
}
