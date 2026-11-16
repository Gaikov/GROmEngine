#include "Engine/Input.h"
#include "Engine/Platform.h"
#include "Engine/GameApp.h"
#include "Engine/console/Console.h"
#include "Core/AppInfo.h"
#include "Core/Memory.h"
#include "Core/Package.h"
#include "Core/buffer/StringWriter.h"
#include "nsLib/log.h"

#include <gtest/gtest.h>
#include <array>
#include <cstring>
#include <memory>

namespace {
class nsMemoryReader final : public IDataReader {
public:
    explicit nsMemoryReader(const std::string& text) : _text(text) {}
    bool Read(void* data, uint size) override {
        if (_pos + size > _text.size()) return false;
        std::memcpy(data, _text.data() + _pos, size);
        _pos += size;
        return true;
    }
    bool Seek(long offset, int origin) override {
        const auto next = (origin == SEEK_CUR ? static_cast<long>(_pos)
            : origin == SEEK_END ? static_cast<long>(_text.size()) : 0) + offset;
        if (next < 0 || next > static_cast<long>(_text.size())) return false;
        _pos = static_cast<size_t>(next);
        return true;
    }
    long Tell() override { return static_cast<long>(_pos); }
    bool IsValid() override { return true; }
private:
    std::string _text;
    size_t _pos = 0;
};

class nsFakeGamepad final : public IGamepad {
public:
    nsFakeGamepad(uint64_t connectionId, int count) : buttons(count), _id(connectionId) {}
    uint64_t GetConnectionId() const override { return _id; }
    int GetAxisCount() const override { return 0; }
    float GetAxisValue(int) const override { return 0; }
    int GetButtonCount() const override { return static_cast<int>(buttons.size()); }
    bool IsButtonPressed(int index) const override { return buttons[index] != 0; }
    std::vector<uint8_t> buttons;
private:
    uint64_t _id;
};

class nsFakeGamepads final : public IGamepads {
public:
    explicit nsFakeGamepads(int& inputQueries) : _inputQueries(inputQueries) {}
    void Init() override {}
    void Release() override { devices.clear(); }
    void Update() override { ++updates; }
    int GetCount() const override { ++_inputQueries; return static_cast<int>(devices.size()); }
    const IGamepad* GetGamepad(int index) override {
        ++_inputQueries;
        return index >= 0 && index < static_cast<int>(devices.size()) ? devices[index].get() : nullptr;
    }

    std::vector<std::unique_ptr<nsFakeGamepad>> devices;
    int updates = 0;

private:
    int& _inputQueries;
};

class nsInputTestPlatform final : public Platform {
public:
    bool Init() override { return true; }
    void Release() override {}
    const nsArgs& GetArgs() override { return _args; }
    bool IsMobile() override { return false; }
    bool ShowInterstitialAd() override { return false; }
    const char* GetDomainName() override { return "test"; }
    void GetCursorPos(int& x, int& y) override { x = y = 0; }
    void SetCursorPos(int, int) override {}
    void ShowCursor(bool) override {}
    bool IsKeyPressed(int key) override {
        ++inputQueries;
        return key >= 0 && key < KEYBOARD_MAX_KEYS && keys[key];
    }
    const char* GetKeyName(int key) override { return key == NS_KEY_SPACE ? "space" : nullptr; }
    ISoftInput* GetSoftInput() override { return nullptr; }
    IGamepads* GetGamepadsProvider() override { return providerAvailable ? &gamepads : nullptr; }
    unsigned int GetTime() override { return 0; }
    void MessagePopup(const char*, const char*) override {}
    void Minimize() override {}
    void SetAppTitle(const char*) override {}
    void GetClientSize(int& width, int& height) override { width = height = 100; }
    void* GetWindowHandler() override { return nullptr; }
    void OpenUrl(const char*) override {}
    void GetDisplayModes(tDisplayModesList&) override {}
    bool ApplyDisplayMode(int, int, bool, int, int) override { return false; }
    IDataReader* FileRead(const char* name) override {
        return std::strcmp(name, "input-default.cfg") == 0 ? new nsMemoryReader(defaultConfig) : nullptr;
    }
    bool FolderListing(const nsFilePath&, nsFilePath::tList&) override { return false; }
    IDataWriter* InternalWrite(const char*) override { return nullptr; }
    IDataReader* InternalRead(const char*) override { return nullptr; }
    ProcAddr GetProcAddr(const char*) override { return nullptr; }
    void SwapBuffers() override {}

    std::array<bool, KEYBOARD_MAX_KEYS> keys{};
    int inputQueries = 0;
    nsFakeGamepads gamepads{inputQueries};
    bool providerAvailable = true;
    std::string defaultConfig;
    std::string currentConfig;
private:
    nsArgs _args;
};

nsInputTestPlatform g_platform;

class nsInputAppInfo final : public IAppInfo {
public:
    const char* GetAppName() override { return "Input_unit"; }
    const char* GetDefCfgPath() override { return "input-default.cfg"; }
    IDataReader* GetCfgReader() override { return new nsMemoryReader(g_platform.currentConfig); }
};

class nsInputWarnings final : public ILogListener {
public:
    void LogPrint(LogLevel level, const char*) override { if (level == PRN_WARNING) ++warnings; }
    int warnings = 0;
};

class InputTest : public testing::Test {
protected:
    void SetUp() override { Initialize({}, {}); }
    void Initialize(const std::string& defaults, const std::string& current) {
        Log::Init();
        g_platform.defaultConfig = defaults;
        g_platform.currentConfig = current;
        g_platform.keys.fill(false);
        g_platform.providerAvailable = true;
        g_platform.gamepads.devices.clear();
        g_platform.gamepads.updates = 0;
        g_platform.inputQueries = 0;
        nsConfig::Init();
        nsConsole::Init();
        x = g_cfg->RegVar("axisx", "0", GVF_INTERNAL);
        y = g_cfg->RegVar("axisy", "0", GVF_INTERNAL);
        g_cfg->RegCmd("tick", [this](int, const char*[]) { ++calls; });
        g_cfg->RegCmd("record", [this](int argc, const char* argv[]) {
            ++calls;
            args.clear();
            for (auto i = 0; i < argc; ++i) args.emplace_back(argv[i]);
        });
        g_cfg->RegCmd("reset", [](int, const char*[]) { g_inp.ReleaseAllKeys(); });
        g_cfg->RegCmd("console", [](int, const char*[]) { nsConsole::Shared()->Activate(true); });
        g_inp.Init();
    }
    void Shutdown() {
        g_inp.Release();
        nsConsole::Release();
        nsConfig::Release();
        g_pack.Release();
        g_platform.gamepads.devices.clear();
    }
    void TearDown() override { Shutdown(); }
    nsFakeGamepad& AddPad(uint64_t id, int buttons = 8) {
        g_platform.gamepads.devices.push_back(std::make_unique<nsFakeGamepad>(id, buttons));
        g_inp.Process();
        return *g_platform.gamepads.devices.back();
    }
    std::string Save() {
        nsStringWriter writer;
        static_cast<ICfgWriter&>(g_inp).OnSaveConfig(&writer);
        return writer.GetBuffer();
    }
    nsVar* x = nullptr;
    nsVar* y = nullptr;
    int calls = 0;
    std::vector<std::string> args;
};
}

Platform* App_GetPlatform() { return &g_platform; }
IGameApp* App_GetGame() { return nullptr; }
IAppInfo* App_GetInfo() { static nsInputAppInfo info; return &info; }

TEST(InputLifecycleTest, ReleaseBeforeInitDoesNotPollThePlatform) {
    const auto queries = g_platform.inputQueries;
    g_inp.Release();
    g_inp.Release();
    EXPECT_EQ(g_platform.inputQueries, queries);
}

TEST_F(InputTest, VariablesAndPressDirectionAreIndependentAcrossPadsAndKeyboard) {
    auto& first = AddPad(10);
    auto& second = AddPad(20);
    g_cfg->ExecLine("padcmd 0 +axisx");
    g_cfg->ExecLine("padcmd 1 axisy");
    g_cfg->ExecLine("keycmd space +axisx");
    first.buttons[0] = second.buttons[0] = true;
    first.buttons[1] = true;
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 3);
    EXPECT_FLOAT_EQ(y->Value(), -1);
    EXPECT_EQ(g_inp.GetVarPressDirection(x), 1);
    EXPECT_EQ(g_inp.GetVarPressDirection(y), -1);
    EXPECT_TRUE(g_inp.WaitAnyKey());
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 3);
    EXPECT_EQ(g_inp.GetVarPressDirection(x), 0);
    first.buttons[0] = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 2);
    g_platform.gamepads.devices.erase(g_platform.gamepads.devices.begin() + 1);
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    first.buttons[1] = false;
    g_platform.keys[NS_KEY_SPACE] = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_FLOAT_EQ(y->Value(), 0);
    EXPECT_EQ(g_platform.gamepads.updates, 0);
}

TEST_F(InputTest, CommandsRepeatOncePerProcessPerDeviceAndKeepArguments) {
    auto& first = AddPad(1);
    auto& second = AddPad(2);
    g_cfg->ExecLine("padcmd 0 record first \"two words\"");
    first.buttons[0] = second.buttons[0] = true;
    g_inp.Process();
    EXPECT_EQ(calls, 2);
    ASSERT_EQ(args.size(), 3);
    EXPECT_EQ(args[1], "first");
    EXPECT_EQ(args[2], "two words");
    g_inp.Process();
    EXPECT_EQ(calls, 2);
    g_cfg->ExecLine("padcmd 1 +tick");
    first.buttons[1] = second.buttons[1] = true;
    for (auto step = 0; step < 4; ++step) g_inp.Process();
    EXPECT_EQ(calls, 10);
    first.buttons[1] = false;
    g_inp.Process();
    EXPECT_EQ(calls, 11);
    g_platform.gamepads.devices.erase(g_platform.gamepads.devices.begin() + 1);
    g_inp.Process();
    EXPECT_EQ(calls, 11);
}

TEST_F(InputTest, MissingProviderIsAnEmptyDeviceListAndKeyboardStillWorks) {
    g_platform.providerAvailable = false;
    g_cfg->ExecLine("padcmd 0 tick");
    g_cfg->ExecLine("keycmd space +axisx");
    g_inp.Process();
    EXPECT_FALSE(g_inp.IsAnyKeyPressed());
    EXPECT_EQ(calls, 0);
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    EXPECT_TRUE(g_inp.IsAnyKeyPressed());
    g_inp.ReleaseAllKeys();
    EXPECT_FLOAT_EQ(x->Value(), 0);
}

TEST_F(InputTest, LosingProviderReleasesOnlyGamepadContributionsAndRearmsOnReturn) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 +axisx");
    g_cfg->ExecLine("padcmd 1 tick");
    g_cfg->ExecLine("keycmd space +axisx");
    pad.buttons[0] = pad.buttons[1] = true;
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    ASSERT_FLOAT_EQ(x->Value(), 2);
    ASSERT_EQ(calls, 1);

    g_platform.providerAvailable = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    EXPECT_EQ(calls, 1);
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);

    g_platform.providerAvailable = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    EXPECT_EQ(calls, 1);
    pad.buttons[0] = pad.buttons[1] = false;
    g_inp.Process();
    pad.buttons[0] = pad.buttons[1] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 2);
    EXPECT_EQ(calls, 2);
}

TEST_F(InputTest, ConnectAndReconnectHeldButtonsWaitForRelease) {
    g_cfg->ExecLine("padcmd 0 +axisx");
    auto pad = std::make_unique<nsFakeGamepad>(1, 8);
    pad->buttons[0] = true;
    g_platform.gamepads.devices.push_back(std::move(pad));
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_FALSE(g_inp.WaitAnyKey());
    EXPECT_TRUE(nsInput::IsAnyKeyPressed());
    g_platform.gamepads.devices[0]->buttons[0] = false;
    g_inp.Process();
    g_platform.gamepads.devices[0]->buttons[0] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    g_platform.gamepads.devices.clear();
    auto next = std::make_unique<nsFakeGamepad>(2, 8);
    next->buttons[0] = true;
    g_platform.gamepads.devices.push_back(std::move(next));
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
}

TEST_F(InputTest, RebindingAndNoneBalanceOldContributionsAndWaitForRelease) {
    auto& first = AddPad(1);
    auto& second = AddPad(2);
    g_cfg->ExecLine("padcmd 0 +axisx");
    first.buttons[0] = second.buttons[0] = true;
    g_inp.Process();
    g_cfg->ExecLine("padcmd 0 +axisy");
    EXPECT_FLOAT_EQ(x->Value(), 0);
    g_inp.Process();
    EXPECT_FLOAT_EQ(y->Value(), 0);
    first.buttons[0] = second.buttons[0] = false;
    g_inp.Process();
    first.buttons[0] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(y->Value(), 1);
    g_cfg->ExecLine("padcmd 0 none");
    EXPECT_FLOAT_EQ(y->Value(), 0);
    g_platform.gamepads.devices.clear();
    g_inp.Process();
    EXPECT_FLOAT_EQ(y->Value(), 0);
}

TEST_F(InputTest, BindingAHeldUnboundButtonWaitsForReleaseWithoutChangingTheVariable) {
    auto& pad = AddPad(1);
    pad.buttons[0] = true;
    g_inp.Process();
    g_cfg->ExecLine("padcmd 0 +axisx");
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    pad.buttons[0] = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    pad.buttons[0] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
    g_cfg->ExecLine("padcmd 0 none");
    EXPECT_FLOAT_EQ(x->Value(), 0);
    pad.buttons[0] = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
}

TEST_F(InputTest, ResetFocusAndConsoleCancelAndDoNotReapplyHeldButtons) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 +axisx");
    for (auto mode = 0; mode < 3; ++mode) {
        pad.buttons[0] = true;
        g_inp.Process();
        ASSERT_FLOAT_EQ(x->Value(), 1);
        if (mode == 0) g_inp.ReleaseAllKeys();
        else if (mode == 1) {
            g_inp.OnActivateApp(false);
            g_inp.Process();
            g_inp.OnActivateApp(true);
        } else {
            nsConsole::Shared()->Activate(true);
            g_inp.Process();
            nsConsole::Shared()->Activate(false);
        }
        EXPECT_FLOAT_EQ(x->Value(), 0);
        g_inp.Process();
        EXPECT_FLOAT_EQ(x->Value(), 0);
        pad.buttons[0] = false;
        g_inp.Process();
    }
    pad.buttons[0] = true;
    g_inp.Process();
    nsConsole::Shared()->Activate(true);
    g_platform.gamepads.devices.clear();
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
}

TEST_F(InputTest, CallbackCanResetOrBlockTheRemainingButtonBatch) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 reset");
    g_cfg->ExecLine("padcmd 1 +axisx");
    pad.buttons[0] = pad.buttons[1] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    pad.buttons[0] = pad.buttons[1] = false;
    g_inp.Process();
    g_cfg->ExecLine("padcmd 0 console");
    pad.buttons[0] = pad.buttons[1] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_TRUE(nsConsole::Shared()->IsActive());
}

TEST_F(InputTest, GameplayResetCancelsHeldBindingsBeforeClearingSharedVariables) {
    auto horizontal = g_cfg->RegVar("move_horz", "0", GVF_INTERNAL);
    auto vertical = g_cfg->RegVar("move_vert", "0", GVF_INTERNAL);
    auto jump = g_cfg->RegVar("jump", "0", GVF_INTERNAL);
    g_cfg->RegCmd("gameplay_reset", [=](int, const char*[]) {
        // Menu/level transitions must cancel bindings before touch clears vars.
        g_inp.ReleaseAllKeys();
        horizontal->SetNumber(0);
        vertical->SetNumber(0);
        jump->SetNumber(0);
    });
    g_cfg->ExecLine("padcmd 0 +move_horz");
    g_cfg->ExecLine("padcmd 1 move_vert");
    g_cfg->ExecLine("padcmd 2 +jump");
    g_cfg->ExecLine("padcmd 3 gameplay_reset");
    g_cfg->ExecLine("padcmd 4 +tick");
    g_cfg->ExecLine("keycmd space +jump");
    auto& first = AddPad(1);
    auto& second = AddPad(2);
    for (auto i = 0; i < 3; ++i) first.buttons[i] = second.buttons[i] = true;
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    ASSERT_FLOAT_EQ(horizontal->Value(), 2);
    ASSERT_FLOAT_EQ(vertical->Value(), -2);
    ASSERT_FLOAT_EQ(jump->Value(), 3);

    first.buttons[3] = first.buttons[4] = true;
    g_inp.Process();
    EXPECT_EQ(calls, 0); // The reset also stops the remainder of this batch.
    for (auto frame = 0; frame < 2; ++frame) {
        g_inp.Process();
        EXPECT_FLOAT_EQ(horizontal->Value(), 0);
        EXPECT_FLOAT_EQ(vertical->Value(), 0);
        EXPECT_FLOAT_EQ(jump->Value(), 0);
        EXPECT_EQ(g_inp.GetVarPressDirection(jump), 0);
    }
    g_platform.gamepads.devices.erase(g_platform.gamepads.devices.begin() + 1); // Detach while held.
    std::fill(first.buttons.begin(), first.buttons.end(), false);
    g_platform.keys[NS_KEY_SPACE] = false;
    g_inp.Process();
    EXPECT_FLOAT_EQ(horizontal->Value(), 0);
    EXPECT_FLOAT_EQ(vertical->Value(), 0);
    EXPECT_FLOAT_EQ(jump->Value(), 0);
    for (auto i = 0; i < 3; ++i) first.buttons[i] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(horizontal->Value(), 1);
    EXPECT_FLOAT_EQ(vertical->Value(), -1);
    EXPECT_FLOAT_EQ(jump->Value(), 1);
    std::fill(first.buttons.begin(), first.buttons.end(), false);
    g_inp.Process();
    EXPECT_FLOAT_EQ(horizontal->Value(), 0);
    EXPECT_FLOAT_EQ(vertical->Value(), 0);
    EXPECT_FLOAT_EQ(jump->Value(), 0);
}

TEST_F(InputTest, CallbackCanGrowBindingsAndUnbindItselfWithoutLosingItsArguments) {
    auto& pad = AddPad(1);
    g_cfg->RegCmd("self", [this](int argc, const char* argv[]) {
        ASSERT_EQ(argc, 2);
        g_cfg->ExecLine("padcmd 4096 +axisx");
        g_cfg->ExecLine("padcmd 0 none");
        EXPECT_STREQ(argv[1], "keep alive");
        ++calls;
    });
    g_cfg->ExecLine("padcmd 0 self \"keep alive\"");
    pad.buttons[0] = true;
    g_inp.Process();
    g_inp.Process();
    EXPECT_EQ(calls, 1);
}

TEST_F(InputTest, QueriesAndInvalidBindingsDoNotChangeTheCurrentBinding) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 +axisx");
    const auto saved = Save();
    for (const auto line : {"padcmd 65535", "padcmd -1 tick", "padcmd 1.2 tick", "padcmd 999999999999999 tick",
            "padcmd 0 missing_command", "padcmd 65536 tick", "padcmd 0 none extra"}) {
        g_cfg->ExecLine(line);
    }
    EXPECT_EQ(Save(), saved);
    pad.buttons[0] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
}

TEST_F(InputTest, PressHistoryGrowsWithDeviceButtonsAndSteadyInputDoesNotAllocate) {
    constexpr auto buttons = 500;
    auto& pad = AddPad(1, buttons);
    for (auto i = 0; i < buttons; ++i) {
        const auto command = std::string("padcmd ") + std::to_string(i) + " +axisx";
        g_cfg->ExecLine(command.c_str());
    }
    g_cfg->ExecLine("padcmd 499 +tick");
    nsInputWarnings listener;
    Log::Shared()->AddPolicy(&listener);
    nsMemory::BeginLoop();
    for (auto frame = 0; frame < 20; ++frame) {
        std::fill(pad.buttons.begin(), pad.buttons.end(), frame / 2 % 2 == 0);
        g_inp.Process();
    }
    nsMemory::EndLoop();
    Log::Shared()->RemovePolicy(&listener);
    EXPECT_EQ(listener.warnings, 0);
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_EQ(g_inp.GetVarPressDirection(x), 0);
    EXPECT_EQ(calls, 10);
}

TEST_F(InputTest, KeyboardUnbindAndGetBindDoNotModifyGamepadBindings) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 +axisx");
    g_cfg->ExecLine("keycmd space +axisx");
    nsString* keys = nullptr;
    EXPECT_EQ(g_inp.GetBind("+axisx", &keys), 1);
    my_free(keys);
    g_inp.Unbind("+axisx");
    pad.buttons[0] = true;
    g_inp.Process();
    EXPECT_FLOAT_EQ(x->Value(), 1);
}

TEST_F(InputTest, ReleaseIsBalancedAndTheConfigWriterDoesNotReadFreedBindings) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 +axisx");
    g_cfg->ExecLine("keycmd space +axisx");
    pad.buttons[0] = true;
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    ASSERT_FLOAT_EQ(x->Value(), 2);
    g_platform.gamepads.devices.clear(); // Native devices may already be unavailable at shutdown.
    const auto queries = g_platform.inputQueries;
    g_inp.Release();
    EXPECT_EQ(g_platform.inputQueries, queries);
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_TRUE(Save().empty());
    g_inp.Release();
    EXPECT_EQ(g_platform.inputQueries, queries);
    EXPECT_FLOAT_EQ(x->Value(), 0);
}

TEST_F(InputTest, ReleaseAfterRuntimeResetDoesNotRemoveContributionsTwice) {
    auto& pad = AddPad(1);
    g_cfg->ExecLine("padcmd 0 axisx");
    g_cfg->ExecLine("keycmd space +axisy");
    pad.buttons[0] = true;
    g_platform.keys[NS_KEY_SPACE] = true;
    g_inp.Process();
    ASSERT_FLOAT_EQ(x->Value(), -1);
    ASSERT_FLOAT_EQ(y->Value(), 1);
    g_inp.ReleaseAllKeys();
    ASSERT_FLOAT_EQ(x->Value(), 0);
    ASSERT_FLOAT_EQ(y->Value(), 0);
    const auto queries = g_platform.inputQueries;
    g_inp.Release();
    EXPECT_EQ(g_platform.inputQueries, queries);
    EXPECT_FLOAT_EQ(x->Value(), 0);
    EXPECT_FLOAT_EQ(y->Value(), 0);
}

TEST_F(InputTest, ConfigRoundTripKeepsNoneArgumentsAndBindingLineBoundaries) {
    g_cfg->ExecLine("padcmd 0 none");
    g_cfg->ExecLine("padcmd 2 +record \"padcmd 0 tick\" \"two words\"");
    const auto saved = Save();
    EXPECT_NE(saved.find("padcmd 0 none"), std::string::npos);
    EXPECT_EQ(saved.find("padcmd 1 "), std::string::npos);
    Shutdown();
    Initialize("padcmd 0 +axisx\npadcmd 3 +axisy\n", saved);
    const auto restored = Save();
    EXPECT_NE(restored.find("padcmd 0 none"), std::string::npos);
    EXPECT_NE(restored.find("padcmd 3 +axisy"), std::string::npos);
    auto& pad = AddPad(1);
    pad.buttons[0] = true;
    g_inp.Process();
    EXPECT_EQ(calls, 0);
    EXPECT_FLOAT_EQ(x->Value(), 0);
    pad.buttons[2] = true;
    g_inp.Process();
    ASSERT_EQ(args.size(), 3);
    EXPECT_EQ(args[1], "padcmd 0 tick");
    EXPECT_EQ(args[2], "two words");
}
