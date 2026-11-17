#include "AndroidGamepads.h"
#include "AndroidGamepadLayout.h"

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

struct nsAndroidKeyControl {
    int button = -1;
    uint8_t dpadKey = 0;
};

struct nsAndroidGamepadDescription {
    int id;
    std::vector<nsAndroidAxisRange> axes;
    std::vector<int> buttons;
    std::vector<nsAndroidAxisRange> mappedAxes;
    std::vector<int> mappedButtons;
    std::array<nsAndroidKeyControl, AKEYCODE_DPAD_DOWN_RIGHT + 1> keyControls{};
    // LT, RT, hat X and hat Y; prepared once, never searched during motion.
    std::array<nsAndroidAxisRange, 4> buttonAxes{};
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

const nsAndroidAxisRange* FindAxis(const nsAndroidGamepadDescription& description, int code) {
    for (const auto& axis : description.axes) if (axis.code == code) return &axis;
    return nullptr;
}

void PrepareLayout(nsAndroidGamepadDescription& description) {
    constexpr int axes[] = {AMOTION_EVENT_AXIS_X, AMOTION_EVENT_AXIS_Y,
        AMOTION_EVENT_AXIS_Z, AMOTION_EVENT_AXIS_RZ};
    for (auto code : axes) {
        auto range = FindAxis(description, code);
        description.mappedAxes.push_back(range ? *range : nsAndroidAxisRange{-1, -1, 1});
    }
    for (const auto& range : description.axes) {
        if (std::find(std::begin(axes), std::end(axes), range.code) != std::end(axes)
            || range.code == AMOTION_EVENT_AXIS_HAT_X || range.code == AMOTION_EVENT_AXIS_HAT_Y) continue;
        if ((range.code == AMOTION_EVENT_AXIS_BRAKE && FindAxis(description, AMOTION_EVENT_AXIS_LTRIGGER))
            || (range.code == AMOTION_EVENT_AXIS_GAS && FindAxis(description, AMOTION_EVENT_AXIS_RTRIGGER))) continue;
        description.mappedAxes.push_back(range);
    }
    description.mappedButtons = {AKEYCODE_BUTTON_A, AKEYCODE_BUTTON_B, AKEYCODE_BUTTON_X, AKEYCODE_BUTTON_Y,
        AKEYCODE_BUTTON_L1, AKEYCODE_BUTTON_R1, AKEYCODE_BUTTON_L2, AKEYCODE_BUTTON_R2,
        AKEYCODE_BUTTON_SELECT, AKEYCODE_BUTTON_START, AKEYCODE_BUTTON_THUMBL, AKEYCODE_BUTTON_THUMBR,
        AKEYCODE_DPAD_UP, AKEYCODE_DPAD_DOWN, AKEYCODE_DPAD_LEFT, AKEYCODE_DPAD_RIGHT, AKEYCODE_BUTTON_MODE};
    constexpr int dpadKeys[] = {AKEYCODE_DPAD_UP, AKEYCODE_DPAD_DOWN, AKEYCODE_DPAD_LEFT, AKEYCODE_DPAD_RIGHT,
        AKEYCODE_DPAD_UP_LEFT, AKEYCODE_DPAD_DOWN_LEFT, AKEYCODE_DPAD_UP_RIGHT, AKEYCODE_DPAD_DOWN_RIGHT};
    for (auto i = 0; i < 8; ++i) description.keyControls[dpadKeys[i]].dpadKey = 1 << i;
    for (auto code : description.buttons) {
        if (code < 0 || code >= static_cast<int>(description.keyControls.size())) continue;
        if (description.keyControls[code].dpadKey) continue;
        if (std::find(description.mappedButtons.begin(), description.mappedButtons.end(), code)
            != description.mappedButtons.end()) continue;
        if (description.mappedButtons.size() == 17) description.mappedButtons.push_back(-1); // Share reserved.
        description.mappedButtons.push_back(code);
    }
    for (auto i = 0; i < static_cast<int>(description.mappedButtons.size()); ++i) {
        const auto code = description.mappedButtons[i];
        if (code >= 0) description.keyControls[code].button = i;
    }
    constexpr int primary[] = {AMOTION_EVENT_AXIS_LTRIGGER, AMOTION_EVENT_AXIS_RTRIGGER,
        AMOTION_EVENT_AXIS_HAT_X, AMOTION_EVENT_AXIS_HAT_Y};
    constexpr int fallback[] = {AMOTION_EVENT_AXIS_BRAKE, AMOTION_EVENT_AXIS_GAS, -1, -1};
    for (auto i = 0; i < 4; ++i) {
        auto range = FindAxis(description, primary[i]);
        if (!range) range = FindAxis(description, fallback[i]);
        description.buttonAxes[i] = range ? *range : nsAndroidAxisRange{-1, -1, 1};
    }
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
        _axes(description.mappedAxes.size()), _buttons(description.mappedButtons.size()) {
        for (auto i = nsGamepadLayout::AxisCount; i < GetAxisCount(); ++i) {
            _axes[i] = NormalizeAxis(0, description.mappedAxes[i]);
        }
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
        if (event.action == AKEY_EVENT_ACTION_DOWN || event.action == AKEY_EVENT_ACTION_UP) {
            UpdateKey(event.keyCode, event.action == AKEY_EVENT_ACTION_DOWN);
        }
    }

    void Update(const GameActivityMotionEvent& event) {
        if ((event.action & AMOTION_EVENT_ACTION_MASK) == AMOTION_EVENT_ACTION_CANCEL) ClearInput();
        else if (event.pointerCount > 0) {
            std::array<float, GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT> values{};
            for (const auto& axis : _description.axes) {
                values[axis.code] = GameActivityPointerAxes_getAxisValue(&event.pointers[0], axis.code);
            }
            UpdateAxes(values);
        }
    }

    void Update(const nsAndroidGamepadInput& input) {
        if (input.clearInput) ClearInput();
        if (input.hasAxes) UpdateAxes(input.axes);
        for (auto code = size_t(0); code < input.buttons.size(); ++code) {
            if (input.touchedButtons[code]) UpdateKey(static_cast<int>(code), input.buttons[code]);
        }
    }

    void ClearInput() {
        _dpadKeys = 0;
        std::fill(_axes.begin(), _axes.end(), 0);
        std::fill(_buttons.begin(), _buttons.end(), 0);
    }

private:
    void UpdateKey(int code, bool down) {
        if (code < 0 || code >= static_cast<int>(_description.keyControls.size())) return;
        const auto& control = _description.keyControls[code];
        if (control.dpadKey) {
            if (down) _dpadKeys |= control.dpadKey;
            else _dpadKeys &= ~control.dpadKey;
            const auto directions = nsAndroidGamepadLayout::DpadDirections(_dpadKeys);
            for (auto i = 0; i < 4; ++i) SetButtonSource(nsGamepadLayout::Up + i, 1, directions & (1 << i));
        } else if (control.button >= 0) SetButtonSource(control.button, 1, down);
    }

    void SetButtonSource(int index, uint8_t source, bool down) {
        if (down) _buttons[index] |= source;
        else _buttons[index] &= ~source;
    }

    void UpdateAxes(const std::array<float, GAME_ACTIVITY_POINTER_INFO_AXIS_COUNT>& values) {
        for (auto i = 0; i < GetAxisCount(); ++i) {
            const auto& range = _description.mappedAxes[i];
            _axes[i] = range.code >= 0 ? NormalizeAxis(values[range.code], range) : 0;
        }
        const auto read = [&](int index) {
            const auto& range = _description.buttonAxes[index];
            return range.code >= 0 ? NormalizeAxis(values[range.code], range) : 0.0f;
        };
        SetButtonSource(nsGamepadLayout::LeftTrigger, 2, read(0) > 0);
        SetButtonSource(nsGamepadLayout::RightTrigger, 2, read(1) > 0);
        const auto x = read(2), y = read(3);
        SetButtonSource(nsGamepadLayout::Up, 2, y < -0.5f);
        SetButtonSource(nsGamepadLayout::Down, 2, y > 0.5f);
        SetButtonSource(nsGamepadLayout::Left, 2, x < -0.5f);
        SetButtonSource(nsGamepadLayout::Right, 2, x > 0.5f);
    }

    uint8_t _dpadKeys = 0;
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
        if (!change.removed && description && _devices.Matches(id, change.name.c_str(),
            static_cast<int>(description->mappedAxes.size()), static_cast<int>(description->mappedButtons.size()))
            && description->axes == change.axes && description->buttons == change.buttons) continue;

        _devices.Disconnect(id); // Destroy the gamepad before its immutable description.
        std::erase_if(_descriptions, [id](const auto& item) { return item->id == id; });
        if (!change.removed) {
            auto next = std::make_unique<nsAndroidGamepadDescription>(nsAndroidGamepadDescription{
                id, std::move(change.axes), std::move(change.buttons)});
            PrepareLayout(*next);
            const auto axes = static_cast<int>(next->mappedAxes.size());
            const auto buttons = static_cast<int>(next->mappedButtons.size());
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
