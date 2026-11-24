#pragma once

#include <span>

#include "IGamepads.h"

class nsGamepadUtils {
public:
    static bool IsAnyKeyPressed(IGamepads *gamepads, std::span<const int> buttons) {
        if (!gamepads) return false;

        const auto count = gamepads->GetCount();
        for (auto index = 0; index < count; ++index) {
            const auto pad = gamepads->GetGamepad(index);
            if (!pad) continue;

            for (const auto button : buttons) {
                if (pad->IsButtonPressed(button)) return true;
            }
        }
        return false;
    }
};
