#pragma once

#import <GameController/GameController.h>
#include "Engine/input/IGamepad.h"
#include <vector>

struct GLFWgamepadstate;

// Connection capabilities and retained native controls, never a second input state.
struct nsAppleGamepadControls {
    // Keep the handle usable by the GLFW fallback on pre-macOS 11 deployments.
    NSObject* profile = nil;
    NSDictionary<NSString*, GCControllerAxisInput*>* nativeAxes = nil;
    NSDictionary<NSString*, GCControllerButtonInput*>* nativeButtons = nil;
    std::vector<GCControllerAxisInput*> axes;
    std::vector<GCControllerButtonInput*> buttons;

    static nsAppleGamepadControls Discover(GCController* controller);
    bool Matches(GCController* controller) const;
};

class nsAppleGamepad final : public IGamepad {
public:
    nsAppleGamepad(uint64_t connectionId, GCController* controller,
        const nsAppleGamepadControls& controls, bool active);
    nsAppleGamepad(uint64_t connectionId, int nativeId, bool active);

    uint64_t GetConnectionId() const override { return _connectionId; }
    int GetAxisCount() const override { return static_cast<int>(_axes.size()); }
    int GetButtonCount() const override { return static_cast<int>(_buttons.size()); }
    float GetAxisValue(int index) const override;
    bool IsButtonPressed(int index) const override;

    void Update(bool active);
    void Update(const GLFWgamepadstate& state);
    void ClearInput();

private:
    uint64_t _connectionId;
    int _nativeId = -1;
    GCController* _controller = nil;
    nsAppleGamepadControls _controls;
    std::vector<float> _axes;
    std::vector<uint8_t> _buttons;
    std::vector<uint8_t> _awaitRelease;

    void UpdateButton(int index, bool down);
};
