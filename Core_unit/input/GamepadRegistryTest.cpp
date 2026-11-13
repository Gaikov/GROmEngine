#include "Platform/input/GamepadRegistry.h"
#include "Engine/input/IGamepad.h"
#include "Core/Memory.h"
#include "nsLib/log.h"

#include <gtest/gtest.h>

namespace {
class nsGamepadLog final : public ILogListener {
public:
    void LogPrint(LogLevel level, const char* message) override {
        if (level == PRN_WARNING) ++warnings;
        messages.emplace_back(message);
    }
    std::vector<std::string> messages;
    int warnings = 0;
};

class nsTestGamepad final : public IGamepad {
public:
    explicit nsTestGamepad(uint64_t id) : _id(id) {}
    uint64_t GetConnectionId() const override { return _id; }
    int GetAxisCount() const override { return 0; }
    int GetButtonCount() const override { return 0; }
    float GetAxisValue(int) const override { return 0; }
    bool IsButtonPressed(int) const override { return false; }
private:
    uint64_t _id;
};

using nsTestRegistry = nsGamepadRegistry<nsTestGamepad>;

class GamepadRegistryTest : public testing::Test {
protected:
    void SetUp() override {
        Log::Init();
        Log::Shared()->AddPolicy(&_log);
    }
    void TearDown() override {
        Log::Shared()->RemovePolicy(&_log);
    }
    nsGamepadLog _log;
};
}

TEST_F(GamepadRegistryTest, SparseNativeIdsStayDenseWithoutMovingSurvivors) {
    nsTestRegistry devices;
    auto& first = devices.Connect(2, "First", 2, 4);
    first.gamepad = std::make_unique<nsTestGamepad>(first.connectionId);
    auto& second = devices.Connect(19, "Second", 6, 12);
    second.gamepad = std::make_unique<nsTestGamepad>(second.connectionId);
    const auto surviving = second.gamepad.get();
    const auto oldId = first.connectionId;
    devices.Disconnect(2);
    ASSERT_EQ(devices.GetCount(), 1);
    EXPECT_EQ(devices.GetDevice(0), &second);
    EXPECT_EQ(devices.GetDevice(0)->gamepad.get(), surviving);
    EXPECT_EQ(devices.GetDevice(1), nullptr);
    auto& reconnected = devices.Connect(2, "First", 2, 4);
    EXPECT_EQ(devices.GetDevice(0)->gamepad.get(), surviving);
    EXPECT_NE(reconnected.connectionId, oldId);
    EXPECT_FALSE(reconnected.gamepad);
}

TEST_F(GamepadRegistryTest, DiscoveryAndDiagnosticsDoNotConstructGamepads) {
    nsTestRegistry devices;
    auto& device = devices.Connect(4, "", 6, 18);
    EXPECT_FALSE(device.gamepad);
    EXPECT_EQ(devices.GetDevice(-1), nullptr);
    EXPECT_EQ(devices.GetDevice(1), nullptr);
    devices.Connect(4, nullptr, 6, 18);
    EXPECT_FALSE(device.gamepad);
    devices.Disconnect(4);
    devices.Disconnect(4);
    ASSERT_EQ(_log.messages.size(), 2);
    EXPECT_EQ(_log.messages[0],
        "Gamepad connected: id=1 name=\"Unknown gamepad\" buttons=18 axes=6\n");
    EXPECT_EQ(_log.messages[1],
        "Gamepad disconnected: id=1 name=\"Unknown gamepad\" buttons=18 axes=6\n");
}

TEST_F(GamepadRegistryTest, CapabilityChangeReplacesConnectionOnce) {
    nsTestRegistry devices;
    auto& first = devices.Connect(3, "Pad", 2, 4);
    const auto id = first.connectionId;
    first.gamepad = std::make_unique<nsTestGamepad>(id);
    auto& next = devices.Connect(3, "Pad", 4, 8);
    EXPECT_NE(next.connectionId, id);
    EXPECT_EQ(next.axisCount, 4);
    EXPECT_EQ(next.buttonCount, 8);
    EXPECT_FALSE(next.gamepad);
    ASSERT_EQ(_log.messages.size(), 3);
    EXPECT_NE(_log.messages[1].find("disconnected"), std::string::npos);
}

TEST_F(GamepadRegistryTest, QueuedShortConnectionLogsBothEventsOnlyWhenPublished) {
    nsTestRegistry devices;
    devices.QueueConnect(8, "Brief pad", 2, 3);
    devices.QueueDisconnect(8);
    EXPECT_EQ(devices.GetCount(), 0);
    EXPECT_TRUE(_log.messages.empty());
    devices.ApplyChanges();
    EXPECT_EQ(devices.GetCount(), 0);
    ASSERT_EQ(_log.messages.size(), 2);
    EXPECT_NE(_log.messages[0].find("connected:"), std::string::npos);
    EXPECT_NE(_log.messages[1].find("disconnected:"), std::string::npos);
    devices.ApplyChanges();
    EXPECT_EQ(_log.messages.size(), 2);
}

TEST_F(GamepadRegistryTest, SteadyDiscoveryKeepsTheBackendInstanceWithoutAllocations) {
    nsTestRegistry devices;
    auto& device = devices.Connect(1, "Pad", 4, 12);
    device.gamepad = std::make_unique<nsTestGamepad>(device.connectionId);
    const auto gamepad = device.gamepad.get();
    nsMemory::BeginLoop();
    bool stable = true;
    for (auto frame = 0; frame < 1000; ++frame) {
        auto& next = devices.Connect(1, "Pad", 4, 12);
        devices.ApplyChanges();
        stable = stable && devices.GetCount() == 1 && next.gamepad.get() == gamepad;
    }
    nsMemory::EndLoop();
    EXPECT_TRUE(stable);
    EXPECT_EQ(_log.warnings, 0);
    EXPECT_EQ(_log.messages.size(), 1);
}
