#pragma once

#include <cstdint>

namespace nsAndroidGamepadLayout {
// Bits follow the discovery table: up, down, left, right, UL, DL, UR, DR.
// Keep individual native keys until release: Up + UL must remain Up when UL releases.
constexpr uint8_t DpadDirections(uint8_t keys) {
    constexpr uint8_t sources[] = {0x51, 0xa2, 0x34, 0xc8};
    uint8_t directions = 0;
    for (auto i = 0; i < 4; ++i) {
        if (keys & sources[i]) directions |= 1 << i;
    }
    return directions;
}
}
