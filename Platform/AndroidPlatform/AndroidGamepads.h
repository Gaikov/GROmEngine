#pragma once

#include "Engine/input/IGamepads.h"
#include "Platform/input/GamepadRegistry.h"

struct GameActivityKeyEvent;
struct GameActivityMotionEvent;
class nsAndroidGamepad;
struct nsAndroidGamepadDescription;
struct nsAndroidGamepadInput;
struct nsAndroidGamepadChange;

class nsAndroidGamepads final : public IGamepads {
public:
    nsAndroidGamepads();
    ~nsAndroidGamepads() override;
    void Init() override {}
    void Update() override;
    void Release() override;
    void ClearInput();
    bool HandleKey(const GameActivityKeyEvent& event);
    bool HandleMotion(const GameActivityMotionEvent& event);
    int GetCount() const override { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index) override;

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
