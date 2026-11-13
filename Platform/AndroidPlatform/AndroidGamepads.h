#pragma once

#include "Engine/input/IGamepad.h"
#include "Platform/input/GamepadRegistry.h"

struct GameActivityKeyEvent;
struct GameActivityMotionEvent;
class nsAndroidGamepad;
struct nsAndroidGamepadDescription;
struct nsAndroidGamepadInput;
struct nsAndroidGamepadChange;

class nsAndroidGamepads {
public:
    nsAndroidGamepads();
    ~nsAndroidGamepads();
    void Update();
    void Release();
    void ClearInput();
    bool HandleKey(const GameActivityKeyEvent& event);
    bool HandleMotion(const GameActivityMotionEvent& event);
    int GetCount() const { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index);

private:
    void CollectDeviceChanges();
    bool HasPendingChange(int nativeId) const;
    nsAndroidGamepadDescription* FindDescription(int nativeId);
    nsAndroidGamepadInput& GetEarlyInput(int nativeId);
    void ApplyEarlyInput(int nativeId, nsAndroidGamepad& gamepad);

    nsGamepadRegistry<nsAndroidGamepad> _devices;
    std::vector<std::unique_ptr<nsAndroidGamepadDescription>> _descriptions;
    std::vector<std::unique_ptr<nsAndroidGamepadInput>> _earlyInputs;
    std::vector<nsAndroidGamepadChange> _changes;
};
