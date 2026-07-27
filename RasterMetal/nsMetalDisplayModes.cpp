//
// Created by OpenCode
//

#include "nsMetalDisplayModes.h"
#include "Core/Config.h"
#include "nsLib/log.h"

nsMetalDisplayModes::nsMetalDisplayModes() : _cfg{32, 800, 600} {
}

bool nsMetalDisplayModes::Init() {
    return true;
}

const rasterConfig_t &nsMetalDisplayModes::GetConfig() const {
    return _cfg;
}

bool nsMetalDisplayModes::ApplyCurrentMode() {
    if (r_fullscreen->Bool()) {
        if (!ApplyFullScreenMode()) {
            r_fullscreen->SetValue(0);
            ApplyWindowMode();
        }
    } else {
        ApplyWindowMode();
    }

    return true;
}

bool nsMetalDisplayModes::ApplyFullScreenMode() {
    auto mode = ValidateCurrentDisplayMode();
    if (!mode) {
        return false;
    }

    Log::Info("Applying full screen mode: %ix%i...", _cfg.currWidth, _cfg.currHeight);
    return App_GetPlatform()->ApplyDisplayMode((int)_cfg.currWidth,
                                               (int)_cfg.currHeight,
                                               true,
                                               mode->bits,
                                               mode->frequency);
}

void nsMetalDisplayModes::ApplyWindowMode() {
    _cfg.currWidth = (uint)r_width->Value();
    _cfg.currHeight = (uint)r_height->Value();

    Log::Info("Applying window mode: %ix%i", _cfg.currWidth, _cfg.currHeight);
    App_GetPlatform()->ApplyDisplayMode((int)_cfg.currWidth,
                                        (int)_cfg.currHeight,
                                        false,
                                        0,
                                        0);
}

const Platform::tDisplayModesList &nsMetalDisplayModes::GetModes() {
    _modes.clear();
    App_GetPlatform()->GetDisplayModes(_modes);
    return _modes;
}

const Platform::DisplayMode *nsMetalDisplayModes::FindValidMode(int width, int height) {
    GetModes();
    for (auto &mode : _modes) {
        if (mode.width == width && mode.height == height) {
            return &mode;
        }
    }
    return nullptr;
}

const Platform::DisplayMode *nsMetalDisplayModes::ValidateCurrentDisplayMode() {
    int w = (int)r_width->Value();
    int h = (int)r_height->Value();

    auto mode = FindValidMode(w, h);
    if (mode) {
        _cfg.currWidth = w;
        _cfg.currHeight = h;
        return mode;
    }

    Log::Warning("Invalid display mode: %i x %i", w, h);

    auto &modes = GetModes();
    if (!modes.empty()) {
        mode = &modes[0];
        r_width->SetValue(_cfg.currWidth = mode->width);
        r_height->SetValue(_cfg.currHeight = mode->height);
        Log::Info("Selected a valid display mode: %i x %i", mode->width, mode->height);
        return mode;
    }

    Log::Warning("Could not find a valid display mode!");
    return nullptr;
}
