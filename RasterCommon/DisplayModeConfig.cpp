#include "DisplayModeConfig.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <tuple>
#include <utility>

#include "Core/Var.h"
#include "Core/sys.h"
#include "DisplayVars.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"

const Platform::DisplayMode *nsDisplayModeConfig::SelectFullscreenMode(
    std::span<const Platform::DisplayMode> modes, int width, int height, int preferredFrequency) {
    const auto score = [preferredFrequency](const Platform::DisplayMode &mode) {
        const auto frequency = std::max(mode.frequency, 0);
        const auto distance = preferredFrequency > 0 ? std::abs(frequency - preferredFrequency) : 0;
        return std::tuple{frequency == 0, distance, -frequency};
    };

    const Platform::DisplayMode *best = nullptr;
    for (const auto &mode : modes) {
        if (mode.fullscreenAvailable && mode.width == width && mode.height == height
            && (!best || score(mode) < score(*best))) {
            best = &mode;
        }
    }
    return best;
}

std::pair<int, int> nsDisplayModeConfig::NormalizeSize(float width, float height) {
    const auto validDimension = [](float value) {
        return std::isfinite(value) && value >= 1 && double(value) <= std::numeric_limits<int>::max();
    };
    if (!validDimension(width) || !validDimension(height)) {
        width = r_width->GetDefaultValue();
        height = r_height->GetDefaultValue();
        if (!validDimension(width) || !validDimension(height)) {
            return {800, 600};
        }
    }
    return {int(width), int(height)};
}

void nsDisplayModeConfig::ApplyConfiguredMode(Platform &platform) {
    auto [width, height] = NormalizeSize(r_width->Value(), r_height->Value());
    auto fullscreen = r_fullscreen->Bool();
    if (fullscreen) {
        Platform::tDisplayModesList modes;
        platform.GetDisplayModes(modes);
        std::erase_if(modes, [](const auto &mode) {
            return !mode.fullscreenAvailable || mode.width <= 0 || mode.height <= 0;
        });
        const auto size = [](const auto &mode) { return std::pair{mode.width, mode.height}; };
        auto mode = std::ranges::find(modes, std::pair{width, height}, size);
        if (mode == modes.end()) {
            Log::Warning("Invalid fullscreen mode: %ix%i", width, height);
            mode = std::ranges::min_element(modes, {}, size);
        }
        fullscreen = mode != modes.end() && platform.ApplyDisplayMode(mode->width, mode->height, true);
        if (fullscreen) {
            width = mode->width;
            height = mode->height;
        } else {
            Log::Warning("Falling back to the default window size");
            const auto windowDefault = NormalizeSize(r_width->GetDefaultValue(), r_height->GetDefaultValue());
            width = windowDefault.first;
            height = windowDefault.second;
        }
    }
    if (!fullscreen && !platform.ApplyDisplayMode(width, height, false)) {
        Sys_FatalError("Could not apply the windowed display mode: %ix%i", width, height);
        return;
    }

    _config.currWidth = width;
    _config.currHeight = height;
    r_fullscreen->SetValue(fullscreen);
    r_width->SetString(StrPrintf("%i", width));
    r_height->SetString(StrPrintf("%i", height));
}
