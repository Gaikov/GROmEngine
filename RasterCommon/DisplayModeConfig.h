#pragma once

#include <span>
#include <utility>

#include "Engine/Platform.h"
#include "Engine/RenDevice.h"

class nsDisplayModeConfig {
public:
    const rasterConfig_t &GetConfig() const { return _config; }
    void ApplyConfiguredMode(Platform &platform);

    static std::pair<int, int> NormalizeSize(float width, float height);
    static const Platform::DisplayMode *SelectFullscreenMode(std::span<const Platform::DisplayMode> modes,
                                                             int width, int height, int preferredFrequency);

private:
    rasterConfig_t _config{32, 800, 600};
};
