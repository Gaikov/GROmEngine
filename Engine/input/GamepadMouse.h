#pragma once

#include "nsLib/Lock.h"
#include "nsLib/SubSystem.h"
#include <cstdint>
#include <vector>

class IGamepads;
struct IUserInput;
class nsInput;
class nsVar;
struct nsEngine;

// Owns UI mouse emulation. Menus only add/remove reasons in enabled;
// engine ingress observes native pointers without changing their delivery.
class nsGamepadMouse final : public nsSubSystem<nsGamepadMouse> {
public:
    nsLock enabled;
    void Update();

protected:
    bool OnInit() override;

private:
    friend struct nsEngine;
    friend class nsInput;
    friend class nsGamepadMouseTestAccess;

    enum class nsPointerEvent { Move, Down, Up };
    struct nsDevice {
        uint64_t id;
        bool seen = false;
        bool clickHeld = false; // Distinguish a disconnected hold from a normal release.
    };

    void UpdateFrame(IGamepads* provider, float seconds);
    void SyncMode();
    bool IsBlocked() const;
    // False if a cancel callback reset input again.
    bool Cancel(bool notify = true);
    void OnPointerEvent(nsPointerEvent event, int id, int x, int y);
    void OnPointerCancel(int id);
    void OnActivateApp(bool active);
    bool GetCursorPos(float& x, float& y);
    void SetCursorPos(float x, float y);
    void SendEscape();
    static float GetAxisState(IGamepads* provider, int index);
    static bool IsButtonPressed(IGamepads* provider, int index);
    void SeedPosition(int width, int height);

    nsVar* _sensitivity = nullptr;
    nsVar* _deadzone = nullptr;
    nsVar* _exponent = nullptr;
    std::vector<nsDevice> _devices;
    // Used only for the synchronous Escape pair, never during ordinary events.
    std::vector<IUserInput*> _escapeRoute;
    std::vector<IUserInput*> _escapeNextRoute;
    bool _buttons[2]{};
    bool _awaitRelease[2]{true, true};
    uint64_t _generation = 0;
    uint64_t _lastUpdate = 0;
    float _x = 0;
    float _y = 0;
    bool _hasPosition = false;
    bool _wasEnabled = false;
    bool _running = false;
    bool _mouseDown = false;
    bool _physicalDown = false;
    bool _appActive = true;
};
