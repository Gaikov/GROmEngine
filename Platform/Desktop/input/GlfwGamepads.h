#pragma once

#include "Engine/input/IGamepads.h"
#include "Platform/input/GamepadRegistry.h"

class nsDesktopGamepad;

class nsGlfwGamepads final : public IGamepads {
public:
    nsGlfwGamepads();
    ~nsGlfwGamepads() override;

    void Init() override;
    void Release() override;
    void Update() override;
    int GetCount() const override { return _devices.GetCount(); }
    const IGamepad* GetGamepad(int index) override;

private:
    nsGamepadRegistry<nsDesktopGamepad> _devices;
    bool _initialized = false;
};
