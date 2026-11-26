#include "Platform/Desktop/input/apple/AppleGamepadDiscovery.h"
#include "Core/Memory.h"
#include <gtest/gtest.h>
#include <limits>
#include "Platform/Desktop/input/GlfwGamepadLayout.h"

// A native profile is usable even without the extended/micro convenience facade.
API_AVAILABLE(macos(11.0))
@interface nsProfileOnlyController : NSObject
@property(nonatomic, strong) GCPhysicalInputProfile* suppliedProfile;
@property(nonatomic, strong) dispatch_queue_t handlerQueue;
@end
@implementation nsProfileOnlyController
- (GCPhysicalInputProfile*)physicalInputProfile { return self.suppliedProfile; }
- (GCExtendedGamepad*)extendedGamepad { return nil; }
- (GCMicroGamepad*)microGamepad { return nil; }
- (NSString*)vendorName { return @"Controller"; }
@end

// Exercise publication without starting IOKit, GLFW or an application UI.
class nsAppleGamepadsTestAccess {
public:
    static void Prepare(nsAppleGamepadDiscovery& discovery) {
        discovery._initialized = discovery._nativeEnabled = discovery._active = true;
    }
    static void Queue(nsAppleGamepadDiscovery& discovery, GCController* controller, bool connected) {
        discovery.QueueController(controller, connected);
    }
    static void Hid(nsAppleGamepadDiscovery& discovery, uint64_t identity, const char* name,
        bool supported, bool synthetic, bool connected = true) {
        discovery._hidChanges.push_back({identity, connected, AppleGamepadGuid(0x45e, 0xb12, 0x100, name),
            name, supported, synthetic});
    }
    static void Glfw(nsAppleGamepadDiscovery& discovery, int id, const char* name, bool connected) {
        discovery._changes.push_back({connected, nil, id, name, AppleGamepadGuid(0x45e, 0xb12, 0x100, name), 4, 18});
    }
    static void Publish(nsAppleGamepadDiscovery& discovery) { discovery.PublishChanges(); }
    static size_t PendingCount(const nsAppleGamepadDiscovery& discovery) { return discovery._changes.size(); }
    static bool IsGlfwPending(const nsAppleGamepadDiscovery& discovery, int id) {
        return discovery._pendingGlfwChanges[id];
    }
    static void Update(nsAppleGamepadDiscovery& discovery, bool active = true) { discovery.UpdateControllers(active); }
    static void InvalidateCapabilities(nsAppleGamepadDiscovery& discovery) {
        discovery._controllers[0].controls.nativeButtons = @{};
    }
    static nsAppleGamepadRouting::Route Route(const nsAppleGamepadDiscovery& discovery, const char* name) {
        const auto guid = AppleGamepadGuid(0x45e, 0xb12, 0x100, name);
        return discovery._routing.GetRoute(guid.data(), name);
    }
    static dispatch_block_t LateCallback(nsAppleGamepadDiscovery& discovery, GCController* controller) {
        discovery._callbackOwner = std::make_shared<nsAppleGamepadDiscovery::nsCallbackOwner>(
            nsAppleGamepadDiscovery::nsCallbackOwner{&discovery});
        auto callbacks = discovery._callbackOwner;
        return ^{ if (auto owner = callbacks->owner) owner->QueueController(controller, true); };
    }
};

namespace {
class nsAppleInputLog final : public ILogListener {
public:
    void LogPrint(LogLevel level, const char* text) override {
        if (level == PRN_WARNING) ++warnings;
        messages.emplace_back(text);
    }
    int warnings = 0;
    std::vector<std::string> messages;
};

class AppleGamepadTest : public testing::Test {
protected:
    void SetUp() override {
        Log::Init();
        Log::Shared()->AddPolicy(&log);
    }
    void TearDown() override {
        Log::Shared()->RemovePolicy(&log);
        Log::Release();
    }
    nsAppleInputLog log;
};
}

TEST_F(AppleGamepadTest, StandardButtonsUseTheExistingXboxConfigIndices) {
    if (@available(macOS 11.0, *)) {
        auto controller = [GCController controllerWithExtendedGamepad];
        auto controls = nsAppleGamepadControls::Discover(controller);
        auto profile = controller.extendedGamepad;
        nsAppleGamepad pad(7, controller, controls, true);
        EXPECT_EQ(pad.GetConnectionId(), 7);
        EXPECT_EQ(pad.GetAxisCount(), 4); // D-pad axes and thumbstick pseudo-buttons are excluded.
        EXPECT_GE(pad.GetButtonCount(), nsGamepadLayout::ButtonCount);
        GCControllerButtonInput* buttons[] = {profile.buttonA, profile.buttonB, profile.buttonX, profile.buttonY,
            profile.leftShoulder, profile.rightShoulder, profile.leftTrigger, profile.rightTrigger,
            profile.buttonOptions, profile.buttonMenu, profile.leftThumbstickButton, profile.rightThumbstickButton,
            nil, nil, nil, nil, profile.buttonHome};
        for (auto index = 0; index < nsGamepadLayout::ButtonCount; ++index) {
            if (index >= nsGamepadLayout::Up && index <= nsGamepadLayout::Right) continue;
            SCOPED_TRACE(index);
            [buttons[index] setValue:1];
            pad.Update(true);
            EXPECT_EQ(pad.IsButtonPressed(index), buttons[index] != nil);
            [buttons[index] setValue:0];
            pad.Update(true);
            EXPECT_FALSE(pad.IsButtonPressed(index));
        }
        const auto dpad = profile.dpad;
        const float directions[][2]{{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
        for (auto index = nsGamepadLayout::Up; index <= nsGamepadLayout::Right; ++index) {
            SCOPED_TRACE(index);
            const auto &direction = directions[index - nsGamepadLayout::Up];
            [dpad setValueForXAxis:direction[0] yAxis:direction[1]];
            pad.Update(true);
            EXPECT_TRUE(pad.IsButtonPressed(index));
            [dpad setValueForXAxis:0 yAxis:0];
            pad.Update(true);
            EXPECT_FALSE(pad.IsButtonPressed(index));
        }
        EXPECT_FALSE(pad.IsButtonPressed(-1));
        EXPECT_FALSE(pad.IsButtonPressed(pad.GetButtonCount()));
        EXPECT_FLOAT_EQ(pad.GetAxisValue(-1), 0);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(pad.GetAxisCount()), 0);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, AxesAndDigitalTriggersUpdateOnlyWhenPublished) {
    if (@available(macOS 11.0, *)) {
        auto controller = [GCController controllerWithExtendedGamepad];
        auto profile = controller.extendedGamepad;
        nsAppleGamepad pad(1, controller, nsAppleGamepadControls::Discover(controller), true);
        [profile.leftThumbstick.xAxis setValue:-0.75f];
        [profile.leftThumbstick.yAxis setValue:0.5f];
        [profile.rightThumbstick.xAxis setValue:0.25f];
        [profile.rightThumbstick.yAxis setValue:-0.5f];
        [profile.leftTrigger setValue:1];
        EXPECT_FLOAT_EQ(pad.GetAxisValue(0), 0);
        EXPECT_FALSE(pad.IsButtonPressed(6));
        pad.Update(true);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(0), -0.75f);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(1), -0.5f);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(2), 0.25f);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(3), 0.5f);
        EXPECT_TRUE(pad.IsButtonPressed(6));
        EXPECT_FALSE(pad.IsButtonPressed(7));
        pad.Update(false);
        EXPECT_FLOAT_EQ(pad.GetAxisValue(0), 0);
        EXPECT_FALSE(pad.IsButtonPressed(6));
        pad.Update(true);
        EXPECT_FALSE(pad.IsButtonPressed(6)); // Focus restoration does not synthesize a new press.
        [profile.leftTrigger setValue:0];
        pad.Update(true);
        [profile.leftTrigger setValue:1];
        pad.Update(true);
        EXPECT_TRUE(pad.IsButtonPressed(6));
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, MissingOptionalSlotsStayNeutralWithoutShiftingOtherButtons) {
    if (@available(macOS 11.0, *)) {
        auto controller = [GCController controllerWithExtendedGamepad];
        auto controls = nsAppleGamepadControls::Discover(controller);
        controls.buttons[8] = controls.buttons[10] = controls.buttons[16] = nil;
        controls.buttons.resize(18);
        auto shareController = [GCController controllerWithExtendedGamepad];
        controls.buttons[17] = shareController.extendedGamepad.buttonA;
        nsAppleGamepad pad(1, controller, controls, true);
        [controller.extendedGamepad.buttonY setValue:1];
        [controller.extendedGamepad.dpad setValueForXAxis:1 yAxis:0];
        [shareController.extendedGamepad.buttonA setValue:1];
        pad.Update(true);
        EXPECT_EQ(pad.GetButtonCount(), 18);
        EXPECT_FALSE(pad.IsButtonPressed(8));
        EXPECT_FALSE(pad.IsButtonPressed(10));
        EXPECT_FALSE(pad.IsButtonPressed(16));
        EXPECT_TRUE(pad.IsButtonPressed(3));
        EXPECT_TRUE(pad.IsButtonPressed(15));
        EXPECT_TRUE(pad.IsButtonPressed(17));
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, GlfwSamplesAreFiniteBoundedAndMissingDataIsNeutral) {
    nsAppleGamepad pad(1, 0, false); // Inactive construction does not poll GLFW.
    GLFWgamepadstate state{};
    state.axes[0] = 2;
    state.axes[1] = -2;
    state.axes[2] = std::numeric_limits<float>::quiet_NaN();
    state.axes[3] = std::numeric_limits<float>::infinity();
    pad.Update(state);
    state.buttons[GLFW_GAMEPAD_BUTTON_A] = GLFW_PRESS;
    pad.Update(state);
    EXPECT_FLOAT_EQ(pad.GetAxisValue(0), 1);
    EXPECT_FLOAT_EQ(pad.GetAxisValue(1), -1);
    EXPECT_FLOAT_EQ(pad.GetAxisValue(2), 0);
    EXPECT_FLOAT_EQ(pad.GetAxisValue(3), 0);
    EXPECT_TRUE(pad.IsButtonPressed(0));
    pad.ClearInput();
    EXPECT_FALSE(pad.IsButtonPressed(0));
    EXPECT_FLOAT_EQ(pad.GetAxisValue(0), 0);
}

TEST_F(AppleGamepadTest, MicroProfileUsesXboxSlotsForItsSupportedControls) {
    if (@available(macOS 11.0, *)) {
        auto controller = [GCController controllerWithMicroGamepad];
        auto controls = nsAppleGamepadControls::Discover(controller);
        EXPECT_EQ(controls.axes.size(), nsGamepadLayout::AxisCount);
        EXPECT_GE(controls.buttons.size(), nsGamepadLayout::ButtonCount);
        EXPECT_TRUE(controls.Matches(controller));
        nsAppleGamepad pad(1, controller, controls, true);
        [controller.microGamepad.buttonA setValue:1];
        pad.Update(true);
        auto pressed = 0;
        for (auto i = 0; i < pad.GetButtonCount(); ++i) pressed += pad.IsButtonPressed(i);
        EXPECT_EQ(pressed, 1);
        EXPECT_TRUE(pad.IsButtonPressed(nsGamepadLayout::A));
        EXPECT_FALSE(pad.IsButtonPressed(nsGamepadLayout::B));
    } else GTEST_SKIP();
}

TEST(GlfwGamepadLayoutTest, NativeButtonsAndTriggersUseTheWebOrder) {
    GLFWgamepadstate state{};
    state.buttons[GLFW_GAMEPAD_BUTTON_START] = GLFW_PRESS;
    state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = GLFW_PRESS;
    state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = GLFW_PRESS;
    state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] = GLFW_PRESS;
    state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] = 1;
    state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] = -1;
    EXPECT_TRUE(GlfwGamepadButton(state, nsGamepadLayout::Menu));
    EXPECT_TRUE(GlfwGamepadButton(state, nsGamepadLayout::LeftStick));
    EXPECT_TRUE(GlfwGamepadButton(state, nsGamepadLayout::Left));
    EXPECT_TRUE(GlfwGamepadButton(state, nsGamepadLayout::Home));
    EXPECT_TRUE(GlfwGamepadButton(state, nsGamepadLayout::LeftTrigger));
    EXPECT_FALSE(GlfwGamepadButton(state, nsGamepadLayout::RightTrigger));
    EXPECT_FALSE(GlfwGamepadButton(state, nsGamepadLayout::A));
    EXPECT_FALSE(GlfwGamepadButton(state, nsGamepadLayout::Right));
}

TEST_F(AppleGamepadTest, DiscoveryIsLazyAndIdenticalControllersHaveIndependentConnections) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        auto first = [GCController controllerWithExtendedGamepad];
        auto second = [GCController controllerWithExtendedGamepad];
        nsAppleGamepadsTestAccess::Queue(discovery, first, true);
        nsAppleGamepadsTestAccess::Queue(discovery, first, true); // Initial scan + notification.
        nsAppleGamepadsTestAccess::Queue(discovery, second, true);
        EXPECT_EQ(devices.GetCount(), 0);
        nsAppleGamepadsTestAccess::Publish(discovery);
        ASSERT_EQ(devices.GetCount(), 2);
        EXPECT_FALSE(devices.GetDevice(0)->gamepad);
        EXPECT_FALSE(devices.GetDevice(1)->gamepad);
        auto firstPad = discovery.GetGamepad(0);
        auto secondPad = discovery.GetGamepad(1);
        const auto oldId = firstPad->GetConnectionId();
        EXPECT_NE(oldId, secondPad->GetConnectionId());
        EXPECT_EQ(discovery.GetGamepad(0), firstPad);
        nsAppleGamepadsTestAccess::Queue(discovery, first, false);
        nsAppleGamepadsTestAccess::Queue(discovery, first, true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(discovery.GetGamepad(0), secondPad);
        EXPECT_NE(discovery.GetGamepad(1)->GetConnectionId(), oldId);
        EXPECT_EQ(discovery.GetGamepad(-1), nullptr);
        EXPECT_EQ(discovery.GetGamepad(2), nullptr);
        EXPECT_EQ(log.messages.size(), 4);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, PhysicalAndSyntheticDuplicatesAreFilteredBeforeDiagnostics) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        auto controller = [GCController controllerWithExtendedGamepad];
        nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Controller", true);
        nsAppleGamepadsTestAccess::Glfw(discovery, 1, "GamePad-1", true);
        nsAppleGamepadsTestAccess::Glfw(discovery, 2, "Generic joystick", true);
        nsAppleGamepadsTestAccess::Queue(discovery, controller, true);
        nsAppleGamepadsTestAccess::Hid(discovery, 10, "Controller", true, false);
        nsAppleGamepadsTestAccess::Hid(discovery, 11, "GamePad-1", false, true);
        nsAppleGamepadsTestAccess::Hid(discovery, 12, "Generic joystick", false, false);
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(devices.GetCount(), 2);
        EXPECT_EQ(devices.Find(0), nullptr);
        EXPECT_EQ(devices.Find(1), nullptr);
        EXPECT_NE(devices.Find(2), nullptr);
        EXPECT_EQ(log.messages.size(), 2);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, SemanticProfileWithoutTypedFacadePublishesTheHidOwnedController) {
    if (@available(macOS 11.0, *)) {
        auto snapshot = [GCController controllerWithExtendedGamepad];
        auto controller = [nsProfileOnlyController new];
        controller.suppliedProfile = snapshot.physicalInputProfile;
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        nsAppleGamepadsTestAccess::Hid(discovery, 1, "Controller", true, false);
        nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Controller", true);
        nsAppleGamepadsTestAccess::Queue(discovery, (GCController*)controller, true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        ASSERT_EQ(devices.GetCount(), 1);
        EXPECT_EQ(devices.Find(0), nullptr);
        auto pad = discovery.GetGamepad(0);
        [snapshot.extendedGamepad.buttonA setValue:1];
        [snapshot.extendedGamepad.buttonB setValue:1];
        [snapshot.extendedGamepad.buttonY setValue:1];
        [snapshot.extendedGamepad.dpad setValueForXAxis:-1 yAxis:1];
        nsAppleGamepadsTestAccess::Update(discovery);
        EXPECT_TRUE(pad->IsButtonPressed(nsGamepadLayout::A));
        EXPECT_TRUE(pad->IsButtonPressed(nsGamepadLayout::B));
        EXPECT_TRUE(pad->IsButtonPressed(nsGamepadLayout::Y));
        EXPECT_TRUE(pad->IsButtonPressed(nsGamepadLayout::Up));
        EXPECT_TRUE(pad->IsButtonPressed(nsGamepadLayout::Left));
        EXPECT_EQ(log.messages.size(), 1);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, ControllerWaitsForATemporarilyUnavailableProfile) {
    if (@available(macOS 11.0, *)) {
        auto controller = [nsProfileOnlyController new];
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        nsAppleGamepadsTestAccess::Queue(discovery, (GCController*)controller, true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(devices.GetCount(), 0);
        EXPECT_TRUE(log.messages.empty());
        auto snapshot = [GCController controllerWithExtendedGamepad];
        controller.suppliedProfile = snapshot.physicalInputProfile;
        nsAppleGamepadsTestAccess::Update(discovery);
        ASSERT_EQ(devices.GetCount(), 1);
        EXPECT_NE(discovery.GetGamepad(0), nullptr);
        EXPECT_EQ(log.messages.size(), 1);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, NativeShortConnectionKeepsItsMetadataWithoutCreatingAnInstance) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        @autoreleasepool {
            auto controller = [GCController controllerWithExtendedGamepad];
            nsAppleGamepadsTestAccess::Queue(discovery, controller, true);
            nsAppleGamepadsTestAccess::Queue(discovery, controller, false);
        }
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(devices.GetCount(), 0);
        ASSERT_EQ(log.messages.size(), 2);
        EXPECT_NE(log.messages[0].find("connected: id=1"), std::string::npos);
        EXPECT_NE(log.messages[1].find("disconnected: id=1"), std::string::npos);
        EXPECT_NE(log.messages[0].find("axes=4"), std::string::npos);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, ReleaseCancelsAlreadyQueuedObserverCallbacks) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        dispatch_block_t callback;
        {
            nsAppleGamepadDiscovery discovery(devices);
            nsAppleGamepadsTestAccess::Prepare(discovery);
            callback = nsAppleGamepadsTestAccess::LateCallback(discovery, [GCController controllerWithExtendedGamepad]);
            discovery.Release();
            discovery.Release();
        }
        callback();
        EXPECT_EQ(devices.GetCount(), 0);
        EXPECT_TRUE(log.messages.empty());
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, UnknownGlfwSlotWaitsForClassificationAndShortConnectionsLogOnce) {
    nsGamepadRegistry<nsAppleGamepad> devices;
    nsAppleGamepadDiscovery discovery(devices);
    nsAppleGamepadsTestAccess::Prepare(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_EQ(devices.GetCount(), 0);
    EXPECT_TRUE(log.messages.empty());
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 1);
    EXPECT_TRUE(nsAppleGamepadsTestAccess::IsGlfwPending(discovery, 0));
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", false);
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false, false);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_EQ(devices.GetCount(), 0);
    ASSERT_EQ(log.messages.size(), 2);
    EXPECT_NE(log.messages[0].find("connected:"), std::string::npos);
    EXPECT_NE(log.messages[1].find("disconnected:"), std::string::npos);
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 0);
    EXPECT_FALSE(nsAppleGamepadsTestAccess::IsGlfwPending(discovery, 0));
    discovery.Release();
    discovery.Release();
}

TEST_F(AppleGamepadTest, DeferredDisconnectWaitsAcrossFramesWithoutBlockingOtherSlots) {
    nsGamepadRegistry<nsAppleGamepad> devices;
    nsAppleGamepadDiscovery discovery(devices);
    nsAppleGamepadsTestAccess::Prepare(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Late joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Late joystick", false);
    nsAppleGamepadsTestAccess::Hid(discovery, 9, "Other joystick", false, false);
    nsAppleGamepadsTestAccess::Glfw(discovery, 1, "Other joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    ASSERT_EQ(devices.GetCount(), 1);
    EXPECT_EQ(devices.GetDevice(0)->nativeId, 1);
    EXPECT_FALSE(devices.GetDevice(0)->gamepad);
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 2);
    ASSERT_EQ(log.messages.size(), 1);

    nsMemory::BeginLoop();
    for (auto frame = 0; frame < 10; ++frame) nsAppleGamepadsTestAccess::Publish(discovery);
    nsMemory::EndLoop();
    EXPECT_EQ(log.warnings, 0);
    EXPECT_EQ(log.messages.size(), 1);
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 2);

    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Late joystick", false, false);
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Late joystick", false, false, false);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_EQ(devices.GetCount(), 1);
    EXPECT_EQ(devices.GetDevice(0)->nativeId, 1);
    ASSERT_EQ(log.messages.size(), 3);
    EXPECT_NE(log.messages[1].find("connected: id=2 name=\"Late joystick\" buttons=18 axes=4"), std::string::npos);
    EXPECT_NE(log.messages[2].find("disconnected: id=2 name=\"Late joystick\" buttons=18 axes=4"), std::string::npos);
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 0);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_EQ(log.messages.size(), 3);
}

TEST_F(AppleGamepadTest, DeferredReconnectKeepsDistinctConnectionsInOrder) {
    nsGamepadRegistry<nsAppleGamepad> devices;
    nsAppleGamepadDiscovery discovery(devices);
    nsAppleGamepadsTestAccess::Prepare(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", false);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_TRUE(log.messages.empty());
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false);
    nsAppleGamepadsTestAccess::Publish(discovery);
    ASSERT_EQ(devices.GetCount(), 1);
    EXPECT_EQ(devices.GetDevice(0)->connectionId, 2);
    EXPECT_FALSE(devices.GetDevice(0)->gamepad);
    ASSERT_EQ(log.messages.size(), 3);
    EXPECT_NE(log.messages[0].find("connected: id=1"), std::string::npos);
    EXPECT_NE(log.messages[1].find("disconnected: id=1"), std::string::npos);
    EXPECT_NE(log.messages[2].find("connected: id=2"), std::string::npos);
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 0);
}

TEST_F(AppleGamepadTest, DeferredGameControllerAndSyntheticDuplicatesNeverPublishGlfwConnections) {
    for (auto synthetic : {false, true}) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Controller", true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Controller", false);
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 2);
        nsAppleGamepadsTestAccess::Hid(discovery, 8, "Controller", !synthetic, synthetic);
        nsAppleGamepadsTestAccess::Hid(discovery, 8, "Controller", !synthetic, synthetic, false);
        nsAppleGamepadsTestAccess::Publish(discovery);
        EXPECT_EQ(devices.GetCount(), 0);
        EXPECT_TRUE(log.messages.empty());
        EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 0);
    }
}

TEST_F(AppleGamepadTest, ReleaseDiscardsDeferredHotplugEvents) {
    nsGamepadRegistry<nsAppleGamepad> devices;
    nsAppleGamepadDiscovery discovery(devices);
    nsAppleGamepadsTestAccess::Prepare(discovery);
    nsAppleGamepadsTestAccess::Glfw(discovery, 0, "Joystick", true);
    nsAppleGamepadsTestAccess::Publish(discovery);
    ASSERT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 1);
    discovery.Release();
    discovery.Release();
    EXPECT_EQ(nsAppleGamepadsTestAccess::PendingCount(discovery), 0);
    EXPECT_FALSE(nsAppleGamepadsTestAccess::IsGlfwPending(discovery, 0));
    EXPECT_EQ(devices.GetCount(), 0);
    EXPECT_TRUE(log.messages.empty());
}

TEST_F(AppleGamepadTest, CapabilityChangeReplacesOnlyTheAffectedConnection) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        nsAppleGamepadsTestAccess::Queue(discovery, [GCController controllerWithExtendedGamepad], true);
        nsAppleGamepadsTestAccess::Queue(discovery, [GCController controllerWithExtendedGamepad], true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        const auto oldId = discovery.GetGamepad(0)->GetConnectionId();
        const auto survivor = discovery.GetGamepad(1);
        nsAppleGamepadsTestAccess::InvalidateCapabilities(discovery);
        nsAppleGamepadsTestAccess::Update(discovery);
        EXPECT_EQ(discovery.GetGamepad(0), survivor);
        EXPECT_NE(discovery.GetGamepad(1)->GetConnectionId(), oldId);
        EXPECT_EQ(log.messages.size(), 4);
    } else GTEST_SKIP();
}

TEST_F(AppleGamepadTest, HidRemoveThenConnectInOneFrameKeepsTheLatestClassification) {
    nsGamepadRegistry<nsAppleGamepad> devices;
    nsAppleGamepadDiscovery discovery(devices);
    nsAppleGamepadsTestAccess::Prepare(discovery);
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false);
    nsAppleGamepadsTestAccess::Publish(discovery);
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false, false);
    nsAppleGamepadsTestAccess::Hid(discovery, 8, "Joystick", false, false);
    nsAppleGamepadsTestAccess::Publish(discovery);
    EXPECT_EQ(nsAppleGamepadsTestAccess::Route(discovery, "Joystick"), nsAppleGamepadRouting::Route::Glfw);
}

TEST_F(AppleGamepadTest, SteadyPublicationDoesNotAllocateOrRecreateTheInstance) {
    if (@available(macOS 11.0, *)) {
        nsGamepadRegistry<nsAppleGamepad> devices;
        nsAppleGamepadDiscovery discovery(devices);
        nsAppleGamepadsTestAccess::Prepare(discovery);
        auto controller = [GCController controllerWithExtendedGamepad];
        nsAppleGamepadsTestAccess::Queue(discovery, controller, true);
        nsAppleGamepadsTestAccess::Publish(discovery);
        auto pad = discovery.GetGamepad(0);
        nsMemory::BeginLoop();
        auto stable = true;
        for (auto i = 0; i < 100; ++i) {
            nsAppleGamepadsTestAccess::Publish(discovery);
            nsAppleGamepadsTestAccess::Update(discovery);
            stable &= discovery.GetGamepad(0) == pad;
        }
        nsMemory::EndLoop();
        EXPECT_TRUE(stable);
        EXPECT_EQ(log.warnings, 0);
        EXPECT_EQ(log.messages.size(), 1);
    } else GTEST_SKIP();
}

TEST(AppleGamepadRoutingTest, CocoaGuidAndNativeDeviceIdentityAreIndependent) {
    const auto guid = AppleGamepadGuid(0x045e, 0x0b12, 0x0509, "Controller");
    EXPECT_STREQ(guid.data(), "030000005e040000120b000009050000");
    nsAppleGamepadRouting routing;
    routing.Connect(1, guid.data(), "Controller", true, false);
    routing.Connect(2, guid.data(), "Controller", true, false);
    routing.Disconnect(1);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Controller"), nsAppleGamepadRouting::Route::GameController);
    routing.Disconnect(2);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Controller"), nsAppleGamepadRouting::Route::Pending);
    routing.Connect(3, guid.data(), "Controller", false, false);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Controller"), nsAppleGamepadRouting::Route::Glfw);
    // A matching GUID alone must not suppress a differently named real device.
    routing.Connect(4, guid.data(), "Synthetic", false, true);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Controller"), nsAppleGamepadRouting::Route::Glfw);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Synthetic"), nsAppleGamepadRouting::Route::GameController);
    routing.Connect(5, guid.data(), "Synthetic", false, false);
    EXPECT_EQ(routing.GetRoute(guid.data(), "Synthetic"), nsAppleGamepadRouting::Route::Pending);
}
