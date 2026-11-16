#pragma once

#include "Engine/input/IGamepads.h"
#include "Platform/input/GamepadRegistry.h"

class nsAppleGamepad;
class nsAppleGamepadDiscovery;

class nsAppleGamepads final : public IGamepads {
public:
    nsAppleGamepads();
    ~nsAppleGamepads() override;

    void Init() override;
    void Release() override;
    void Update() override;
    int GetCount() const override { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index) override;

private:
    nsGamepadRegistry<nsAppleGamepad> _devices;
    std::unique_ptr<nsAppleGamepadDiscovery> _discovery;
};
