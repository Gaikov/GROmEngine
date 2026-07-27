//
// Created by OpenCode
//
#pragma once

#include "Engine/Platform.h"
#include "Engine/RenDevice.h"

class nsMetalDisplayModes {
public:
    nsMetalDisplayModes();

    bool Init();
    bool ApplyCurrentMode();
    const rasterConfig_t &GetConfig() const;
    const Platform::tDisplayModesList &GetModes();

private:
    Platform::tDisplayModesList _modes;
    rasterConfig_t _cfg;

    const Platform::DisplayMode *FindValidMode(int width, int height);
    const Platform::DisplayMode *ValidateCurrentDisplayMode();
    bool ApplyFullScreenMode();
    void ApplyWindowMode();
};
