// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#include "ScissorRect.h"

#include <algorithm>
#include <cmath>

nsRect nsScissorRect::Normalize(const nsRect &rect, int targetWidth, int targetHeight) {
    if (targetWidth <= 0 || targetHeight <= 0 || !std::isfinite(rect.x) || !std::isfinite(rect.y)
        || !std::isfinite(rect.width) || !std::isfinite(rect.height) || rect.width <= 0 || rect.height <= 0) return {};
    // Add in double and clamp before conversion: finite float extents may overflow
    // float addition or lie outside the integer range accepted by native APIs.
    const auto left = std::floor(std::clamp(double(rect.x), 0.0, double(targetWidth)));
    const auto top = std::floor(std::clamp(double(rect.y), 0.0, double(targetHeight)));
    const auto right = std::ceil(std::clamp(double(rect.x) + rect.width, 0.0, double(targetWidth)));
    const auto bottom = std::ceil(std::clamp(double(rect.y) + rect.height, 0.0, double(targetHeight)));
    if (right <= left || bottom <= top) return {};
    return {float(left), float(top), float(right - left), float(bottom - top)};
}
