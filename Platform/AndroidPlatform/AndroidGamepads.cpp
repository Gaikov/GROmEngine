#include "AndroidGamepads.h"

#include <array>
#include <cmath>
#include <mutex>
#include <android/input.h>
#include <android/keycodes.h>
#include <jni.h>
#include <game-activity/GameActivity.h>

struct nsAndroidAxisRange {
    int code;
    float min;
    float max;
    bool operator==(const nsAndroidAxisRange&) const = default;
};

struct nsAndroidGamepadChange {
    int id = 0;
    bool removed = false;
    std::string name;
    std::vector<nsAndroidAxisRange> axes;
    std::vector<int> buttons;
};

struct nsAndroidGamepadDescription {
    int id;
    std::vector<nsAndroidAxisRange> axes;
    std::vector<int> buttons;
};

namespace {
std::mutex g_deviceMutex;
std::vector<nsAndroidGamepadChange> g_deviceChanges;

float NormalizeAxis(float value, const nsAndroidAxisRange& range) {
    if (!std::isfinite(value) || !std::isfinite(range.min)
        || !std::isfinite(range.max) || range.max <= range.min) return 0;
    const auto normalized = 2.0 * (static_cast<double>(value) - range.min)
        / (static_cast<double>(range.max) - range.min) - 1.0;
    return static_cast<float>(std::clamp(normalized, -1.0, 1.0));
}

bool IsControllerKey(int code) {
    return (code >= AKEYCODE_BUTTON_A && code <= AKEYCODE_BUTTON_MODE)
        || (code >= AKEYCODE_BUTTON_1 && code <= AKEYCODE_BUTTON_16)
        || (code >= AKEYCODE_DPAD_UP && code <= AKEYCODE_DPAD_CENTER)
        || (code >= AKEYCODE_DPAD_UP_LEFT && code <= AKEYCODE_DPAD_DOWN_RIGHT);
}
}

// Only exists before lazy construction or while topology changes are pending.
// Touched controls preserve events without overwriting unrelated held input.
struct nsAndroidGamepadInput {
    int id;
    bool clearInput = false;
    bool hasAxes = false;
    std::array<float, GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT> axes{};
    std::array<uint8_t, AKEYCODE_DPAD_DOWN_RIGHT + 1> buttons{};
    std::array<uint8_t, AKEYCODE_DPAD_DOWN_RIGHT + 1> touchedButtons{};

    explicit nsAndroidGamepadInput(int nativeId) : id(nativeId) {}

    void Cancel() {
        clearInput = true;
        hasAxes = false;
        touchedButtons.fill(0);
    }

    void Update(const GameActivityKeyEvent& event) {
        if (event.keyCode < 0 || event.keyCode >= static_cast<int>(buttons.size())) return;
        if (event.action == AKEY_EVENT_ACTION_DOWN || event.action == AKEY_EVENT_ACTION_UP) {
            buttons[event.keyCode] = event.action == AKEY_EVENT_ACTION_DOWN;
            touchedButtons[event.keyCode] = true;
        }
    }

    void Update(const GameActivityMotionEvent& event) {
        if ((event.action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_CANCEL) Cancel();
        else if (event.pointerCount > 0) {
            hasAxes = true;
            for (auto axis = 0; axis < GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT; ++axis) {
                axes[axis] = GameActivityPointerAxes_getAxisValue(&event.pointers[0], axis);
            }
        }
    }
};

class nsAndroidGamepad final : public IGamepad {
public:
    nsAndroidGamepad(uint64_t connectionId, const nsAndroidGamepadDescription& description) :
        _connectionId(connectionId), _description(description),
        _axes(description.axes.size()), _buttons(description.buttons.size()) {
        for (auto i = 0; i < GetAxisCount(); ++i) _axes[i] = NormalizeAxis(0, description.axes[i]);
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

    void Update(const GameActivityKeyEvent& event) {
        const auto& codes = _description.buttons;
        const auto it = std::lower_bound(codes.begin(), codes.end(), event.keyCode);
        if (it != codes.end() && *it == event.keyCode
            && (event.action == AKEY_EVENT_ACTION_DOWN || event.action == AKEY_EVENT_ACTION_UP)) {
            _buttons[it - codes.begin()] = event.action == AKEY_EVENT_ACTION_DOWN;
        }
    }

    void Update(const GameActivityMotionEvent& event) {
        if ((event.action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_CANCEL) ClearInput();
        else if (event.pointerCount > 0) {
            for (auto i = 0; i < GetAxisCount(); ++i) {
                const auto& range = _description.axes[i];
                _axes[i] = NormalizeAxis(GameActivityPointerAxes_getAxisValue(&event.pointers[0], range.code), range);
            }
        }
    }

    void Update(const nsAndroidGamepadInput& input) {
        if (input.clearInput) ClearInput();
        if (input.hasAxes) {
            for (auto i = 0; i < GetAxisCount(); ++i) {
                const auto& range = _description.axes[i];
                _axes[i] = NormalizeAxis(input.axes[range.code], range);
            }
        }
        for (auto i = 0; i < GetButtonCount(); ++i) {
            const auto code = _description.buttons[i];
            if (code >= 0 && code < static_cast<int>(input.buttons.size()) && input.touchedButtons[code]) {
                _buttons[i] = input.buttons[code];
            }
        }
    }

    void ClearInput() {
        std::fill(_axes.begin(), _axes.end(), 0);
        std::fill(_buttons.begin(), _buttons.end(), 0);
    }

private:
    uint64_t _connectionId;
    const nsAndroidGamepadDescription& _description;
    std::vector<float> _axes;
    std::vector<uint8_t> _buttons;
};

extern "C" JNIEXPORT void JNICALL
Java_com_grom_engine_GamepadBridge_nativeDeviceChanged(JNIEnv* env, jclass,
        jint id, jstring name, jintArray axes, jfloatArray minima,
        jfloatArray maxima, jintArray buttons) {
    nsAndroidGamepadChange change;
    change.id = id;
    if (name) {
        const auto chars = env->GetStringUTFChars(name, nullptr);
        if (!chars) return;
        change.name = chars;
        env->ReleaseStringUTFChars(name, chars);
    }
    if (!axes || !minima || !maxima || !buttons) return;
    const auto count = env->GetArrayLength(axes);
    if (env->GetArrayLength(minima) != count || env->GetArrayLength(maxima) != count) return;
    std::vector<jint> codes(count);
    std::vector<jfloat> minValues(count), maxValues(count);
    if (count) {
        env->GetIntArrayRegion(axes, 0, count, codes.data());
        env->GetFloatArrayRegion(minima, 0, count, minValues.data());
        env->GetFloatArrayRegion(maxima, 0, count, maxValues.data());
    }
    for (auto i = 0; i < count; ++i) {
        if (codes[i] >= 0 && codes[i] < GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT) {
            change.axes.push_back({codes[i], minValues[i], maxValues[i]});
        }
    }
    std::sort(change.axes.begin(), change.axes.end(),
        [](const auto& a, const auto& b) { return a.code < b.code; });
    change.buttons.resize(env->GetArrayLength(buttons));
    if (!change.buttons.empty()) {
        env->GetIntArrayRegion(buttons, 0, static_cast<jsize>(change.buttons.size()), change.buttons.data());
        std::sort(change.buttons.begin(), change.buttons.end());
    }
    if (env->ExceptionCheck()) return;
    const auto lock = std::lock_guard(g_deviceMutex);
    g_deviceChanges.push_back(std::move(change));
}

extern "C" JNIEXPORT void JNICALL
Java_com_grom_engine_GamepadBridge_nativeDeviceRemoved(JNIEnv*, jclass, jint id) {
    nsAndroidGamepadChange change;
    change.id = id;
    change.removed = true;
    const auto lock = std::lock_guard(g_deviceMutex);
    g_deviceChanges.push_back(std::move(change));
}

nsAndroidGamepads::nsAndroidGamepads() {
    for (auto axis = 0; axis < GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT; ++axis) {
        GameActivityPointerAxes_enableAxis(axis);
    }
}

nsAndroidGamepads::~nsAndroidGamepads() {
    Release();
}

nsAndroidGamepadDescription* nsAndroidGamepads::FindDescription(int nativeId) {
    for (auto& description : _descriptions) {
        if (description->id == nativeId) return description.get();
    }
    return nullptr;
}

nsAndroidGamepadInput& nsAndroidGamepads::GetEarlyInput(int nativeId) {
    for (auto& input : _earlyInputs) {
        if (input->id == nativeId) return *input;
    }
    _earlyInputs.push_back(std::make_unique<nsAndroidGamepadInput>(nativeId));
    return *_earlyInputs.back();
}

void nsAndroidGamepads::ApplyEarlyInput(int nativeId, nsAndroidGamepad& gamepad) {
    for (auto it = _earlyInputs.begin(); it != _earlyInputs.end(); ++it) {
        if ((*it)->id == nativeId) {
            gamepad.Update(**it);
            _earlyInputs.erase(it);
            return;
        }
    }
}

const IGamepad* nsAndroidGamepads::GetGamepad(int index) {
    const auto device = _devices.GetDevice(index);
    if (!device) return nullptr;
    if (!device->gamepad) {
        const auto description = FindDescription(device->nativeId);
        if (!description) return nullptr;
        device->gamepad = std::make_unique<nsAndroidGamepad>(device->connectionId, *description);
        if (!HasPendingChange(device->nativeId)) ApplyEarlyInput(device->nativeId, *device->gamepad);
    }
    return device->gamepad.get();
}

bool nsAndroidGamepads::HasPendingChange(int nativeId) const {
    for (const auto& change : _changes) if (change.id == nativeId) return true;
    return false;
}

void nsAndroidGamepads::CollectDeviceChanges() {
    std::vector<nsAndroidGamepadChange> changes;
    {
        const auto lock = std::lock_guard(g_deviceMutex);
        if (g_deviceChanges.empty()) return;
        changes.swap(g_deviceChanges);
    }
    for (auto& change : changes) {
        if (change.removed) {
            std::erase_if(_earlyInputs, [&change](const auto& input) { return input->id == change.id; });
        }
        _changes.push_back(std::move(change));
    }
}

void nsAndroidGamepads::Update() {
    CollectDeviceChanges();
    for (auto& change : _changes) {
        const auto id = change.id;
        const auto description = FindDescription(id);
        const auto axes = static_cast<int>(change.axes.size());
        const auto buttons = static_cast<int>(change.buttons.size());
        if (!change.removed && description && _devices.Matches(id, change.name.c_str(), axes, buttons)
            && description->axes == change.axes && description->buttons == change.buttons) continue;

        _devices.Disconnect(id); // Destroy the gamepad before its immutable description.
        std::erase_if(_descriptions, [id](const auto& item) { return item->id == id; });
        if (!change.removed) {
            auto next = std::make_unique<nsAndroidGamepadDescription>(nsAndroidGamepadDescription{
                id, std::move(change.axes), std::move(change.buttons)});
            _descriptions.push_back(std::move(next));
            _devices.Connect(id, change.name.c_str(), axes, buttons);
        }
    }
    _changes.clear();
    for (auto i = 0; i < _devices.GetCount(); ++i) {
        const auto device = _devices.GetDevice(i);
        if (device->gamepad) ApplyEarlyInput(device->nativeId, *device->gamepad);
    }
}

void nsAndroidGamepads::ClearInput() {
    for (auto i = 0; i < _devices.GetCount(); ++i) {
        const auto device = _devices.GetDevice(i);
        if (device->gamepad) device->gamepad->ClearInput();
    }
    for (auto& input : _earlyInputs) input->Cancel();
}

void nsAndroidGamepads::Release() {
    _devices.Clear();
    _descriptions.clear();
    _earlyInputs.clear();
    _changes.clear();
}

bool nsAndroidGamepads::HandleKey(const GameActivityKeyEvent& event) {
    CollectDeviceChanges();
    const auto device = _devices.Find(event.deviceId);
    const auto controllerSource = (event.source & AINPUT_SOURCE_GAMEPAD) == AINPUT_SOURCE_GAMEPAD;
    const auto pending = std::find_if(_changes.rbegin(), _changes.rend(), [&event](const auto& change) {
        return change.id == event.deviceId;
    });
    const auto discovered = pending != _changes.rend() && !pending->removed;
    if (!IsControllerKey(event.keyCode) || (!device && !controllerSource && !discovered)) return false;
    if (device && device->gamepad && !HasPendingChange(event.deviceId)) device->gamepad->Update(event);
    else GetEarlyInput(event.deviceId).Update(event);
    return true;
}

bool nsAndroidGamepads::HandleMotion(const GameActivityMotionEvent& event) {
    if ((event.source & AINPUT_SOURCE_CLASS_MASK) != AINPUT_SOURCE_CLASS_JOYSTICK) return false;
    CollectDeviceChanges();
    const auto device = _devices.Find(event.deviceId);
    if (device && device->gamepad && !HasPendingChange(event.deviceId)) device->gamepad->Update(event);
    else GetEarlyInput(event.deviceId).Update(event);
    return true;
}
