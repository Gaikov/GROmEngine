#include "Engine/input/GamepadMouse.h"
#include "Engine/input/IGamepads.h"
#include "Engine/input/MultiUserInput.h"
#include "Engine/engine.h"
#include "Engine/EngineContext.h"
#include "Engine/Input.h"
#include "Engine/console/Console.h"
#include "Core/Memory.h"
#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <limits>
#include <memory>

void InputTestClientSize(int width, int height);
void InputTestMousePlatform(bool mobile);
void InputTestCursor(int x, int y);
int InputTestCursorWrites();

class nsGamepadMouseTestAccess {
public:
    static void Frame(IGamepads* provider, float seconds = 0.1f) {
        nsGamepadMouse::Shared()->UpdateFrame(provider, seconds);
    }
    static void Pointer(char event, int id, int x, int y) {
        const auto type = event == 'd' ? nsGamepadMouse::nsPointerEvent::Down
            : event == 'u' ? nsGamepadMouse::nsPointerEvent::Up : nsGamepadMouse::nsPointerEvent::Move;
        nsGamepadMouse::Shared()->OnPointerEvent(type, id, x, y);
    }
    static void Cancel(int id) {
        nsGamepadMouse::Shared()->OnPointerCancel(id);
    }
    static void Focus(bool active) { nsGamepadMouse::Shared()->OnActivateApp(active); }
};

namespace {
struct nsMouseEvent { char type; int id; int x; int y; };
std::array<nsMouseEvent, 4096> events;
size_t eventCount = 0;
int routeQueries = 0;
int routeVisits = 0;
int nativePointerEvents = 0;
int nativeKeyEvents = 0;
bool cancelOnDown = false;
bool resetOnMove = false;
IUserInput* activeInput = nullptr;
IUserInput* gameInput = nullptr;

void Record(char type, int id, int x = 0, int y = 0) {
    if (eventCount < events.size()) events[eventCount++] = {type, id, x, y};
}
int Count(char type) {
    auto result = 0;
    for (auto i = size_t(0); i < eventCount; ++i) result += events[i].type == type;
    return result;
}

class nsMousePad final : public IGamepad {
public:
    explicit nsMousePad(uint64_t id) : _id(id) {}
    uint64_t GetConnectionId() const override { return _id; }
    int GetAxisCount() const override { return 4; }
    float GetAxisValue(int axis) const override { return axis >= 0 && axis < 4 ? axes[axis] : 0; }
    int GetButtonCount() const override { return 17; }
    bool IsButtonPressed(int button) const override { return button >= 0 && button < 2 && buttons[button]; }
    std::array<float, 4> axes{};
    std::array<bool, 2> buttons{};
private:
    uint64_t _id;
};
class nsMousePads final : public IGamepads {
public:
    void Init() override {}
    void Release() override { devices.clear(); }
    void Update() override { ++updates; }
    int GetCount() const override { return static_cast<int>(devices.size()); }
    const IGamepad* GetGamepad(int index) override { return devices[index].get(); }
    nsMousePad& Add(uint64_t id) {
        devices.push_back(std::make_unique<nsMousePad>(id));
        return *devices.back();
    }
    std::vector<std::unique_ptr<nsMousePad>> devices;
    int updates = 0;
};
class nsRoutingReceiver final : public IUserInput {
public:
    void OnKeyDown(int key, bool repeat, int mods) override {
        Record('k', key, repeat, mods);
        ++downs;
        if (replacement) {
            route->ClearInputs();
            route->AddInput(replacement);
        }
        if (resetOnDown) g_inp.ReleaseAllKeys();
    }
    void OnKeyUp(int key, int mods) override { Record('K', key, 0, mods); ++ups; }
    void OnChar(char) override {}
    bool OnPointerDown(float x, float y, int id) override {
        Record('d', id, static_cast<int>(x), static_cast<int>(y));
        captured = id;
        if (cancelOnDown) nsEngine::OnPointerCancel(id);
        return true;
    }
    bool OnPointerUp(float x, float y, int id) override {
        Record('u', id, static_cast<int>(x), static_cast<int>(y));
        if (captured == id) { ++clicks; captured = -1; }
        return true;
    }
    bool OnPointerMove(float x, float y, int id) override {
        Record('m', id, static_cast<int>(x), static_cast<int>(y));
        if (moveReplacement) {
            route->ClearInputs();
            route->AddInput(moveReplacement);
        }
        if (resetOnMove) g_inp.ReleaseAllKeys();
        return true;
    }
    void OnPointerCancel(int id) override {
        if (captured != id) return;
        captured = -1;
        ++cancels;
        if (cancelReplacement) activeInput = cancelReplacement;
        if (resetOnCancel) g_inp.ReleaseAllKeys();
    }
    bool OnMouseWheel(float) override { return true; }
    int downs = 0, ups = 0, clicks = 0, cancels = 0, captured = -1;
    nsMultiUserInput* route = nullptr;
    IUserInput* replacement = nullptr;
    IUserInput* moveReplacement = nullptr;
    IUserInput* cancelReplacement = nullptr;
    bool resetOnDown = false;
    bool resetOnCancel = false;
};
class nsObservedRoute final : public nsMultiUserInput {
public:
    tInputsList& GetInteractiveItems() override {
        ++routeVisits;
        return nsMultiUserInput::GetInteractiveItems();
    }
};
class nsMouseWarnings final : public ILogListener {
public:
    void LogPrint(LogLevel level, const char*) override { if (level == PRN_WARNING) ++count; }
    int count = 0;
};
class GamepadMouseTest : public testing::Test {
protected:
    void SetUp() override {
        InputTestMousePlatform(false);
        InputTestCursor(50, 50);
        Log::Init();
        nsConfig::Init();
        nsConsole::Init();
        ASSERT_TRUE(nsEngineContext::Init());
        ASSERT_TRUE(nsGamepadMouse::Init());
        EXPECT_FLOAT_EQ(g_cfg->FindVar("pad_ui_sensitivity")->Value(), 2);
        EXPECT_FLOAT_EQ(g_cfg->FindVar("pad_ui_deadzone")->Value(), 0.05f);
        EXPECT_FLOAT_EQ(g_cfg->FindVar("pad_ui_exponent")->Value(), 0.5f);
        // Reference-curve tests keep explicit parameters independent of defaults.
        g_cfg->ExecLine("pad_ui_sensitivity 1");
        g_cfg->ExecLine("pad_ui_deadzone 0.2");
        g_cfg->ExecLine("pad_ui_exponent 2");
        Mouse().enabled.AddLock("menu"); // Available before game/nsInput initialization.
        g_inp.Init();
        g_inp.ShowCursor(false);
        nested.AddInput(&first);
        root.AddInput(&nested);
        first.route = &nested;
        gameInput = activeInput = &root;
        cancelOnDown = resetOnMove = false;
        pads.Add(1);
        Frame(0);
        eventCount = routeQueries = routeVisits = nativePointerEvents = nativeKeyEvents = 0;
    }
    void TearDown() override {
        const auto eventsBeforeRelease = eventCount;
        const auto queriesBeforeRelease = routeQueries;
        g_inp.Release();
        gameInput = activeInput = nullptr;
        Mouse().enabled.RemoveLock("menu"); // Game release can still use enabled.
        nsGamepadMouse::Release();
        nsGamepadMouse::Release();
        nsEngineContext::Release();
        EXPECT_EQ(eventCount, eventsBeforeRelease);
        EXPECT_EQ(routeQueries, queriesBeforeRelease);
        nsConsole::Release();
        nsConfig::Release();
        InputTestMousePlatform(false);
    }
    void Frame(float seconds = 0.1f) { nsGamepadMouseTestAccess::Frame(&pads, seconds); }
    nsMousePad& Pad() { return *pads.devices.front(); }
    static nsGamepadMouse& Mouse() { return *nsGamepadMouse::Shared(); }
    static void Position(float expectedX, float expectedY) {
        float x = 0, y = 0;
        g_inp.GetCursorPos(x, y);
        EXPECT_NEAR(x, expectedX, 0.001f);
        EXPECT_NEAR(y, expectedY, 0.001f);
    }
    nsMousePads pads;
    nsObservedRoute root, nested;
    nsRoutingReceiver first, second;
};
}

// Headless context provides the routing boundary without initializing rendering.
bool nsEngineContext::OnInit() { return true; }
void nsEngineContext::OnRelease() {}
IUserInput* nsEngineContext::GetActiveInput() { ++routeQueries; return activeInput; }
void nsEngineContext::CancelPointer(int id) {
    Record('c', id);
    if (gameInput) gameInput->OnPointerCancel(id);
}
void nsEngineContext::OnKeyDown(int key, bool repeat, int mods) { GetActiveInput()->OnKeyDown(key, repeat, mods); }
void nsEngineContext::OnKeyUp(int key, int mods) { GetActiveInput()->OnKeyUp(key, mods); }
void nsEngineContext::OnChar(char ch) { GetActiveInput()->OnChar(ch); }
nsVisualContext2d::~nsVisualContext2d() = default;

// Native entrypoints observe physical events before the ordinary UI delivery.
// Emulation must reach the receiver directly, without calling these entrypoints.
IUserInput* nsEngine::GetActiveInput() { return nsEngineContext::Shared()->GetActiveInput(); }
void nsEngine::OnPointerMove(int id, int x, int y) {
    ++nativePointerEvents;
    nsGamepadMouseTestAccess::Pointer('m', id, x, y);
    if (activeInput) activeInput->OnPointerMove(x, y, id);
}
void nsEngine::OnPointerDown(int id, int x, int y) {
    ++nativePointerEvents;
    nsGamepadMouseTestAccess::Pointer('d', id, x, y);
    if (activeInput) activeInput->OnPointerDown(x, y, id);
}
void nsEngine::OnPointerUp(int id, int x, int y) {
    ++nativePointerEvents;
    nsGamepadMouseTestAccess::Pointer('u', id, x, y);
    if (activeInput) activeInput->OnPointerUp(x, y, id);
}
void nsEngine::OnPointerCancel(int id) {
    ++nativePointerEvents;
    nsGamepadMouseTestAccess::Cancel(id);
    nsEngineContext::Shared()->CancelPointer(id);
}
void nsEngine::OnKeyDown(int key, bool repeat, int mods) {
    ++nativeKeyEvents;
    if (activeInput) activeInput->OnKeyDown(key, repeat, mods);
}
void nsEngine::OnKeyUp(int key, int mods) {
    ++nativeKeyEvents;
    if (activeInput) activeInput->OnKeyUp(key, mods);
}

TEST_F(GamepadMouseTest, RadialDeadzoneAndPowerCurveKeepFractionalPosition) {
    Pad().axes[0] = 0.2f;
    Frame();
    EXPECT_EQ(eventCount, 0);
    Pad().axes[0] = 0.6f;
    Frame();
    Position(52.5f, 50);
    Frame();
    Position(55, 50);
    EXPECT_EQ(pads.updates, 0);
}
TEST_F(GamepadMouseTest, DefaultSettingsUseFivePercentDeadzoneAndSquareRootResponse) {
    g_cfg->ExecLine("pad_ui_sensitivity 2");
    g_cfg->ExecLine("pad_ui_deadzone 0.05");
    g_cfg->ExecLine("pad_ui_exponent 0.5");
    Pad().axes[0] = 0.05f;
    Frame();
    EXPECT_EQ(Count('m'), 0);
    Pad().axes[0] = 0.2875f; // Rescaled magnitude 0.25 -> square-root response 0.5.
    Frame();
    Position(60, 50);
}
TEST_F(GamepadMouseTest, DiagonalIsNotFasterAndPositiveYMovesDown) {
    Pad().axes[0] = Pad().axes[1] = 1;
    Frame();
    Position(50 + 10 / std::sqrt(2.0f), 50 + 10 / std::sqrt(2.0f));
}
TEST_F(GamepadMouseTest, TimeIsUnscaledBoundedAndPositionIsClamped) {
    Pad().axes[0] = 1;
    Frame(10);
    Position(60, 50);
    Frame(0.05f);
    Frame(0.05f);
    Position(70, 50);
    g_inp.SetCursorPos(99, 1); // GUI coordinates -> client (99,99).
    Pad().axes[1] = 1;
    Frame();
    Position(99, 99);
    g_cfg->ExecLine("pad_ui_sensitivity 0");
    Pad().axes[0] = Pad().axes[1] = -1;
    Frame();
    Position(99, 99);
    EXPECT_EQ(InputTestCursorWrites(), 0);
}
TEST_F(GamepadMouseTest, ClickDoesNotRepeatOrGenerateArtificialMove) {
    Pad().buttons[0] = true;
    Frame();
    ASSERT_EQ(Count('d'), 1);
    EXPECT_EQ(first.captured, NS_POINTER_PRIMARY);
    EXPECT_EQ(Count('m'), 0);
    for (auto frame = 0; frame < 3; ++frame) Frame();
    Pad().buttons[0] = false;
    Frame();
    EXPECT_EQ(Count('d'), 1);
    EXPECT_EQ(Count('u'), 1);
    EXPECT_EQ(first.clicks, 1);
    EXPECT_EQ(nativePointerEvents, 0);
    for (auto i = size_t(0); i < eventCount; ++i) EXPECT_EQ(events[i].id, NS_POINTER_PRIMARY);
}
TEST_F(GamepadMouseTest, EmulatedMoveClickAndCancelBypassNativeIngress) {
    Pad().axes[0] = 1;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    Mouse().enabled.RemoveLock("menu");
    Frame();
    EXPECT_GT(Count('m'), 0);
    EXPECT_EQ(Count('d'), 2);
    EXPECT_EQ(Count('u'), 1);
    EXPECT_EQ(Count('c'), 1);
    EXPECT_EQ(first.clicks, 1);
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(nativePointerEvents, 0);
}
TEST_F(GamepadMouseTest, EnablingWithDeflectedStickDoesNotEmitMoveUntilPositionChanges) {
    Mouse().enabled.RemoveLock("menu");
    Frame();
    Pad().axes[0] = 1;
    Mouse().enabled.AddLock("menu");
    Frame();
    EXPECT_EQ(Count('m'), 0);
    Position(50, 50);
    Frame();
    EXPECT_EQ(Count('m'), 1);
    Position(60, 50);
}
TEST_F(GamepadMouseTest, ResizeClampsAndZeroSizeCancelsWithoutClick) {
    Pad().axes[0] = 1;
    Frame();
    InputTestClientSize(40, 30);
    Frame();
    Position(39, 29);
    Pad().buttons[0] = true;
    Frame();
    InputTestClientSize(0, 0);
    Frame();
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(first.clicks, 0);
}
TEST_F(GamepadMouseTest, NonFiniteParametersAndSamplesCannotPoisonPosition) {
    g_cfg->FindVar("pad_ui_sensitivity")->SetValue(std::numeric_limits<float>::infinity());
    g_cfg->FindVar("pad_ui_deadzone")->SetValue(std::numeric_limits<float>::quiet_NaN());
    g_cfg->FindVar("pad_ui_exponent")->SetValue(std::numeric_limits<float>::quiet_NaN());
    Pad().axes[0] = 1;
    Frame();
    Position(70, 50);
    Pad().axes[0] = std::numeric_limits<float>::quiet_NaN();
    Pad().axes[1] = std::numeric_limits<float>::infinity();
    Frame();
    Position(70, 50);
}
TEST_F(GamepadMouseTest, EscapeIsCompleteWithoutRepeatAndInspectsRouteOnlyOnPress) {
    Pad().buttons[1] = true;
    Frame();
    ASSERT_EQ(Count('k'), 1);
    ASSERT_EQ(Count('K'), 1);
    EXPECT_EQ(events[0].id, NS_KEY_ESCAPE);
    EXPECT_EQ(events[0].x, 0);
    EXPECT_EQ(routeQueries, 2);
    EXPECT_EQ(nativeKeyEvents, 0);
    for (auto frame = 0; frame < 3; ++frame) Frame();
    Pad().buttons[1] = false;
    Frame();
    EXPECT_EQ(Count('m'), 0);
    EXPECT_EQ(Count('k'), 1);
    EXPECT_EQ(Count('K'), 1);
    EXPECT_EQ(routeQueries, 2);
}
TEST_F(GamepadMouseTest, EscapeDoesNotReleaseIntoReplacementNestedReceiver) {
    first.replacement = &second;
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(first.downs, 1);
    EXPECT_EQ(first.ups, 0);
    EXPECT_EQ(second.ups, 0);
    EXPECT_EQ(Count('K'), 0);
}
TEST_F(GamepadMouseTest, ResetInEscapeDownSuppressesUp) {
    first.resetOnDown = true;
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(first.downs, 1);
    EXPECT_EQ(first.ups, 0);
}
TEST_F(GamepadMouseTest, EscapeCancelsHeldClickAndStillDeliversItsOwnPair) {
    Pad().buttons[0] = true;
    Frame();
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(first.clicks, 0);
    EXPECT_EQ(Count('k'), 1);
    EXPECT_EQ(Count('K'), 1);
    Pad().buttons[0] = Pad().buttons[1] = false;
    Frame();
    EXPECT_EQ(Count('u'), 0);
}
TEST_F(GamepadMouseTest, ResetDuringDragCancelSuppressesEscapeIntoTheReplacementRoute) {
    Pad().buttons[0] = true;
    Frame();
    first.resetOnCancel = true;
    first.cancelReplacement = &second;
    const auto queries = routeQueries;
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(Count('c'), 1);
    EXPECT_EQ(Count('k'), 0);
    EXPECT_EQ(Count('K'), 0);
    EXPECT_EQ(routeQueries, queries);
    EXPECT_EQ(first.downs, 0);
    EXPECT_EQ(second.downs, 0);
    EXPECT_EQ(first.clicks, 0);
    for (auto frame = 0; frame < 3; ++frame) Frame();
    EXPECT_EQ(Count('k'), 0);
    Pad().buttons[0] = Pad().buttons[1] = false;
    Frame();
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(second.downs, 1);
    EXPECT_EQ(second.ups, 1);
    EXPECT_EQ(nativeKeyEvents, 0);
}
TEST_F(GamepadMouseTest, AnotherPadCannotStealAHeldDrag) {
    auto& other = pads.Add(2);
    Frame(0);
    Pad().buttons[0] = true;
    Frame();
    other.axes[0] = 1;
    other.buttons[0] = true;
    Frame();
    Position(50, 50);
    EXPECT_EQ(Count('d'), 1);
    Pad().buttons[0] = false;
    Frame();
    EXPECT_EQ(Count('u'), 1);
    other.axes[0] = 0;
    other.buttons[0] = false;
    Frame();
    other.axes[0] = 1;
    Frame();
    Position(60, 50);
}
TEST_F(GamepadMouseTest, DisconnectAndReconnectHeldButtonRearm) {
    Pad().buttons[0] = true;
    Frame();
    pads.devices.clear();
    Frame();
    EXPECT_EQ(first.cancels, 1);
    auto& next = pads.Add(2);
    next.buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 1);
    next.buttons[0] = false;
    Frame();
    next.buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 2);
}
TEST_F(GamepadMouseTest, PhysicalTakeoverCancelsDragBeforeDeliveryAndSynchronizesPosition) {
    Pad().buttons[0] = true;
    Frame();
    nsEngine::OnPointerMove(NS_POINTER_PRIMARY, 20, 30);
    ASSERT_GE(eventCount, 3);
    EXPECT_EQ(events[eventCount - 2].type, 'c');
    EXPECT_EQ(events[eventCount - 1].type, 'm');
    Position(20, 30);
    EXPECT_EQ(first.cancels, 1);
    Frame();
    EXPECT_EQ(Count('d'), 1);
}
TEST_F(GamepadMouseTest, PhysicalCancelWaitsForButtonsPressedAfterTheLastSample) {
    Pad().buttons[0] = true;
    Frame();
    Pad().buttons[1] = true; // Pressed after the emulator's last sample.
    nsEngine::OnPointerMove(NS_POINTER_PRIMARY, 20, 30);
    Frame();
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(Count('d'), 1);
    EXPECT_EQ(Count('u'), 0);
    EXPECT_EQ(Count('k'), 0);
    EXPECT_EQ(Count('K'), 0);
    Pad().buttons[1] = false;
    Frame();
    Pad().buttons[1] = true;
    Frame();
    EXPECT_EQ(Count('k'), 1);
    EXPECT_EQ(Count('K'), 1);
    EXPECT_EQ(Count('d'), 1); // A remains blocked independently until its own release.
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 2);
}
TEST_F(GamepadMouseTest, PhysicalCancelBlocksANewClickUntilANeutralSample) {
    Pad().buttons[0] = true; // The previous sample still says released.
    nsEngine::OnPointerMove(NS_POINTER_PRIMARY, 20, 30);
    Frame();
    EXPECT_EQ(Count('d'), 0);
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 1);
}
TEST_F(GamepadMouseTest, PhysicalDragBlocksPadAndNativeUpIsNeverFiltered) {
    nsEngine::OnPointerDown(NS_POINTER_PRIMARY, 40, 50);
    Pad().buttons[0] = true;
    Pad().axes[0] = 1;
    Frame();
    EXPECT_EQ(Count('d'), 1);
    EXPECT_EQ(first.cancels, 0);
    nsEngine::OnPointerUp(NS_POINTER_PRIMARY, 40, 50);
    nsEngine::OnPointerUp(NS_POINTER_PRIMARY, 40, 50); // Unmatched native event is still delivered.
    EXPECT_EQ(Count('u'), 2);
    Frame();
    EXPECT_EQ(Count('d'), 1);
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 2);
}
TEST_F(GamepadMouseTest, PhysicalDownDuringEmulatedDragRemainsBlockedUntilNativeUp) {
    Pad().buttons[0] = true;
    Frame();
    nsEngine::OnPointerDown(NS_POINTER_PRIMARY, 20, 30);
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(Count('d'), 2); // One emulated press and one physical press.
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Pad().axes[0] = 1;
    Frame();
    Position(20, 30);
    EXPECT_EQ(Count('d'), 2);
    EXPECT_EQ(Count('u'), 0);
    nsEngine::OnPointerUp(NS_POINTER_PRIMARY, 20, 30);
    Frame();
    EXPECT_EQ(Count('d'), 2); // Held pad button still requires release.
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 3);
}
TEST_F(GamepadMouseTest, KeyboardActivityDoesNotCancelMouseDrag) {
    Pad().buttons[0] = true;
    Frame();
    nsEngine::OnKeyDown(NS_KEY_SPACE, false, 0);
    nsEngine::OnKeyUp(NS_KEY_SPACE, 0);
    EXPECT_EQ(first.cancels, 0);
    Pad().buttons[0] = false;
    Frame();
    EXPECT_EQ(first.clicks, 1);
}
TEST_F(GamepadMouseTest, DisabledCursorUsesSystemPositionAndSetterWithoutMovingOnSwitch) {
    Pad().axes[0] = 1;
    Frame();
    Position(60, 50);
    Mouse().enabled.RemoveLock("menu");
    const auto moves = Count('m');
    Position(50, 50);
    Frame();
    EXPECT_EQ(Count('m'), moves);
    g_inp.SetCursorPos(20, 30);
    EXPECT_EQ(InputTestCursorWrites(), 1);
    Position(20, 70);
    Mouse().enabled.AddLock("menu");
    Position(20, 70);
    EXPECT_EQ(InputTestCursorWrites(), 1);
}
TEST_F(GamepadMouseTest, InitialHeldButtonsAndMultipleEnableReasonsRearm) {
    Mouse().enabled.RemoveLock("menu");
    Frame();
    Pad().buttons[0] = Pad().buttons[1] = true;
    Mouse().enabled.AddLock("menu");
    Mouse().enabled.AddLock("dialog");
    Frame();
    EXPECT_EQ(Count('d'), 0);
    EXPECT_EQ(Count('k'), 0);
    Mouse().enabled.RemoveLock("dialog");
    Pad().buttons[0] = Pad().buttons[1] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('d'), 1);
}
TEST_F(GamepadMouseTest, ConsoleFocusAndMissingProviderCancelWithoutClick) {
    Pad().buttons[0] = true;
    Frame();
    nsConsole::Shared()->Activate(true);
    EXPECT_EQ(first.cancels, 0); // Console manages only its own state.
    Frame();
    EXPECT_EQ(first.cancels, 1);
    nsConsole::Shared()->Activate(false);
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    nsGamepadMouseTestAccess::Focus(false);
    EXPECT_EQ(first.cancels, 2);
    nsGamepadMouseTestAccess::Focus(true);
    Frame();
    EXPECT_EQ(Count('d'), 2);
    Pad().buttons[0] = false;
    Frame();
    Pad().buttons[0] = true;
    Frame();
    nsGamepadMouseTestAccess::Frame(nullptr);
    EXPECT_EQ(first.cancels, 3);
    EXPECT_EQ(first.clicks, 0);
}
TEST_F(GamepadMouseTest, ConsoleCancellationReleasesCaptureOutsideTheActiveRoute) {
    Pad().buttons[0] = true;
    Frame();
    ASSERT_EQ(first.captured, NS_POINTER_PRIMARY);
    activeInput = &second;
    nsConsole::Shared()->Activate(true);
    Frame();
    EXPECT_EQ(first.captured, -1);
    EXPECT_EQ(first.cancels, 1);
    EXPECT_EQ(second.cancels, 0);
    EXPECT_EQ(nativePointerEvents, 0);
}
TEST_F(GamepadMouseTest, CallbackCancellationStopsTheOldBatch) {
    resetOnMove = true;
    Pad().axes[0] = 1;
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(Count('m'), 1);
    EXPECT_EQ(Count('d'), 0);
    resetOnMove = false;
    Pad().axes[0] = 0;
    Pad().buttons[0] = false;
    Frame();
    cancelOnDown = true;
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(first.cancels, 1);
    Pad().buttons[0] = false;
    Frame();
    EXPECT_EQ(first.clicks, 0);
}
TEST_F(GamepadMouseTest, MoveCallbackRefreshesTheRouteBeforeButtonDown) {
    first.moveReplacement = &second;
    Pad().axes[0] = 1;
    Pad().buttons[0] = true;
    Frame();
    EXPECT_EQ(first.captured, -1);
    EXPECT_EQ(second.captured, NS_POINTER_PRIMARY);
    EXPECT_EQ(Count('m'), 1);
    EXPECT_EQ(Count('d'), 1);
    EXPECT_EQ(routeQueries, 2);
    EXPECT_EQ(nativePointerEvents, 0);
}
TEST_F(GamepadMouseTest, MobileCursorUsesEnabledAndStartsAtCenter) {
    Mouse().enabled.RemoveLock("menu");
    Frame();
    InputTestMousePlatform(true);
    InputTestCursor(10, 20);
    EXPECT_FALSE(g_inp.CanDrawCursor());
    Mouse().enabled.AddLock("menu");
    EXPECT_TRUE(g_inp.CanDrawCursor());
    Position(50, 50);
    nsEngine::OnPointerMove(NS_POINTER_SECONDARY, 20, 30);
    Position(20, 30);
    EXPECT_TRUE(g_inp.CanDrawCursor());
}
TEST_F(GamepadMouseTest, SteadyMovementDoesNotAllocateOrScanInputRoute) {
    Pad().axes[0] = 1;
    Frame();
    nsMouseWarnings warnings;
    Log::Shared()->AddPolicy(&warnings);
    nsMemory::BeginLoop();
    for (auto frame = 0; frame < 100; ++frame) Frame(0.001f);
    nsMemory::EndLoop();
    Log::Shared()->RemovePolicy(&warnings);
    EXPECT_EQ(warnings.count, 0);
    EXPECT_EQ(routeQueries, Count('m'));
    EXPECT_EQ(routeVisits, 2 * Count('m')); // Only normal delivery through root/nested.
    EXPECT_EQ(nativePointerEvents, 0);
    EXPECT_GT(Count('m'), 1);
}
