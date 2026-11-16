#pragma once

#include "Engine/input/IGamepads.h"
#include "Platform/input/GamepadRegistry.h"

class nsWebGamepad;

class nsWebGamepads final : public IGamepads {
public:
    nsWebGamepads();
    ~nsWebGamepads() override;

    void Init() override;
    void Release() override;
    void Update() override;
    int GetCount() const override { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index) override;

private:
    nsGamepadRegistry<nsWebGamepad> _devices;
    bool _initialized = false;
    bool _sampleAvailable = false;
};
