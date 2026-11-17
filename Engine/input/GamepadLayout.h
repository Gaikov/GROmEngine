#pragma once

// The Xbox/web order exposed by every public IGamepad, including decorators.
struct nsGamepadLayout {
    static constexpr int LeftX = 0;
    static constexpr int LeftY = 1;
    static constexpr int RightX = 2;
    static constexpr int RightY = 3;
    static constexpr int AxisCount = 4;

    static constexpr int A = 0;
    static constexpr int B = 1;
    static constexpr int X = 2;
    static constexpr int Y = 3;
    static constexpr int LeftShoulder = 4;
    static constexpr int RightShoulder = 5;
    static constexpr int LeftTrigger = 6;
    static constexpr int RightTrigger = 7;
    static constexpr int View = 8;
    static constexpr int Menu = 9;
    static constexpr int LeftStick = 10;
    static constexpr int RightStick = 11;
    static constexpr int Up = 12;
    static constexpr int Down = 13;
    static constexpr int Left = 14;
    static constexpr int Right = 15;
    static constexpr int Home = 16;
    static constexpr int ButtonCount = 17;
};
