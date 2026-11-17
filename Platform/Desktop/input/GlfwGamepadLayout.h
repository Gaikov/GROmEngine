#pragma once

#include "Engine/input/GamepadLayout.h"
#include <algorithm>
#include <cmath>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

inline float GlfwGamepadAxis(const GLFWgamepadstate& state, int index) {
    auto value = state.axes[index];
    return std::isfinite(value) ? std::clamp(value, -1.0f, 1.0f) : 0;
}

inline bool GlfwGamepadButton(const GLFWgamepadstate& state, int index) {
    constexpr int buttons[] = {GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_B,
        GLFW_GAMEPAD_BUTTON_X, GLFW_GAMEPAD_BUTTON_Y, GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
        GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, -1, -1, GLFW_GAMEPAD_BUTTON_BACK,
        GLFW_GAMEPAD_BUTTON_START, GLFW_GAMEPAD_BUTTON_LEFT_THUMB, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
        GLFW_GAMEPAD_BUTTON_DPAD_UP, GLFW_GAMEPAD_BUTTON_DPAD_DOWN, GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
        GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, GLFW_GAMEPAD_BUTTON_GUIDE};
    if (index == nsGamepadLayout::LeftTrigger) return GlfwGamepadAxis(state, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) > 0;
    if (index == nsGamepadLayout::RightTrigger) return GlfwGamepadAxis(state, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) > 0;
    return state.buttons[buttons[index]] == GLFW_PRESS;
}
