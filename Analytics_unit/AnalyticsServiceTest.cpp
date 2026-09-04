#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "Analytics/AnalyticsEvent.h"
#include "Analytics/AnalyticsService.h"
#include "Core/AppInfo.h"
#include "Core/Config.h"
#include "Engine/Platform.h"
#include "Http/HttpBackend.h"
#include "nsLib/log.h"

namespace {
std::string g_savedConfig;

class MemoryReader final : public IDataReader {
public:
    explicit MemoryReader(std::string data) : _data(std::move(data)) {}

    bool Read(void *data, uint size) override {
        if (_position + size > _data.size()) return false;
        std::memcpy(data, _data.data() + _position, size);
        _position += size;
        return true;
    }

    bool Seek(long offset, int origin) override {
        long position = offset;
        if (origin == SEEK_CUR) position += static_cast<long>(_position);
        if (origin == SEEK_END) position += static_cast<long>(_data.size());
        if (position < 0 || static_cast<size_t>(position) > _data.size()) return false;
        _position = static_cast<size_t>(position);
        return true;
    }

    long Tell() override { return static_cast<long>(_position); }
    bool IsValid() override { return true; }

private:
    std::string _data;
    size_t _position = 0;
};

class MemoryWriter final : public IDataWriter {
public:
    bool Printf(const char *fmt, ...) override {
        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        const auto written = std::vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
        if (written < 0 || static_cast<size_t>(written) >= sizeof(buffer)) return false;
        g_savedConfig.append(buffer, static_cast<size_t>(written));
        return true;
    }

    bool Write(const void *data, uint size) override {
        g_savedConfig.append(static_cast<const char *>(data), size);
        return true;
    }

    bool IsValid() override { return true; }
    bool Seek(long, int) override { return false; }
    long Tell() override { return static_cast<long>(g_savedConfig.size()); }
};

class TestAppInfo final : public IAppInfo {
public:
    const char *GetAppName() override { return "analytics_unit"; }
    const char *GetDefCfgPath() override { return nullptr; }
    IDataReader *GetCfgReader() override {
        return g_savedConfig.empty() ? nullptr : new MemoryReader(g_savedConfig);
    }
    IDataWriter *GetCfgWriter() override {
        g_savedConfig.clear();
        return new MemoryWriter();
    }
};

class TestPlatform final : public Platform {
public:
    bool Init() override { return true; }
    void Release() override {}
    const nsArgs &GetArgs() override { return _args; }
    bool IsMobile() override { return false; }
    bool ShowInterstitialAd() override { return false; }
    const char *GetDomainName() override { return "test"; }
    void GetCursorPos(int &x, int &y) override { x = y = 0; }
    void SetCursorPos(int, int) override {}
    void ShowCursor(bool) override {}
    bool IsKeyPressed(int) override { return false; }
    const char *GetKeyName(int) override { return ""; }
    ISoftInput *GetSoftInput() override { return nullptr; }
    unsigned int GetTime() override { return 0; }
    void MessagePopup(const char *, const char *) override {}
    void Minimize() override {}
    void SetAppTitle(const char *) override {}
    void GetClientSize(int &width, int &height) override { width = height = 0; }
    void *GetWindowHandler() override { return nullptr; }
    void OpenUrl(const char *) override {}
    void GetDisplayModes(tDisplayModesList &) override {}
    bool ApplyDisplayMode(int, int, bool, int, int) override { return false; }
    IDataReader *FileRead(const char *) override { return nullptr; }
    bool FolderListing(const nsFilePath &, nsFilePath::tList &) override { return false; }
    IDataWriter *InternalWrite(const char *) override { return new MemoryWriter(); }
    IDataReader *InternalRead(const char *) override { return nullptr; }
    ProcAddr GetProcAddr(const char *) override { return nullptr; }
    void SwapBuffers() override {}

private:
    nsArgs _args;
};

class FakeBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        lastRequestId = requestId;
        lastRequest = request;
        ++sendCount;
        return true;
    }

    void CancelAll() override { cancelled = true; }

    RequestId lastRequestId = 0;
    nsHttpRequest lastRequest;
    unsigned int sendCount = 0;
    bool cancelled = false;
};

TestAppInfo g_appInfo;
TestPlatform g_platform;

nsAnalyticsConfig ValidConfig() {
    nsAnalyticsConfig config;
    config.endpointUrl = "https://example.test/events";
    config.appId = "test-game";
    config.debugAppIdSuffix = "_debug";
    config.location = "test";
    return config;
}

class AnalyticsServiceTest : public testing::Test {
protected:
    void SetUp() override {
        g_savedConfig.clear();
        ASSERT_TRUE(Log::Init());
        ASSERT_TRUE(nsConfig::Init());
        backend = new FakeBackend();
        ASSERT_TRUE(nsHttpClient::Init(backend));
        ASSERT_TRUE(nsAnalyticsService::Init(ValidConfig()));
    }

    void TearDown() override {
        nsAnalyticsService::Release();
        nsHttpClient::Release();
        nsConfig::Release();
        Log::Release();
        g_savedConfig.clear();
    }

    FakeBackend *backend = nullptr;
};
}

IAppInfo *App_GetInfo() {
    return &g_appInfo;
}

Platform *App_GetPlatform() {
    return &g_platform;
}

TEST(AnalyticsEvent, FormatsParametersInStableOrder) {
    const std::map<std::string, std::string> params = {
        {"level", "3"},
        {"event", "started"}
    };
    EXPECT_EQ("levelAction;event=started;level=3",
              nsAnalyticsEvent::Format("levelAction", params));
}

TEST(AnalyticsEvent, EscapesEventAndJsonValues) {
    EXPECT_EQ("action;value=a%3Bb%3Dc%20%2F",
              nsAnalyticsEvent::Format("action", {{"value", "a;b=c /"}}));
    EXPECT_EQ("{\"appId\":\"game\\\"id\",\"location\":\"test\","
              "\"event\":\"line\\nnext\"}",
              nsAnalyticsEvent::BuildPayload("game\"id", "test", "line\nnext"));
}

TEST(AnalyticsEvent, EnforcesServerLengthLimit) {
    EXPECT_TRUE(nsAnalyticsEvent::IsValid(std::string(128, 'a')));
    EXPECT_FALSE(nsAnalyticsEvent::IsValid(std::string(129, 'a')));
    EXPECT_FALSE(nsAnalyticsEvent::IsValid(""));
}

TEST(AnalyticsEvent, AppliesDebugSuffixOnlyToDebugBuilds) {
    EXPECT_EQ("game_debug", nsAnalyticsEvent::BuildAppId("game", "_debug", true));
    EXPECT_EQ("game", nsAnalyticsEvent::BuildAppId("game", "_debug", false));
}

TEST_F(AnalyticsServiceTest, RejectsMissingEndpointOrAppId) {
    nsAnalyticsService::Release();
    EXPECT_FALSE(nsAnalyticsService::Init({}));
    auto config = ValidConfig();
    config.appId.clear();
    EXPECT_FALSE(nsAnalyticsService::Init(config));
}

TEST_F(AnalyticsServiceTest, SendsConfiguredPayloadAndDebugAppId) {
    nsAnalyticsService::Shared()->SendEvent("app_open");
    nsAnalyticsService::Shared()->Update(0);
    ASSERT_EQ(1u, backend->sendCount);
    EXPECT_EQ("https://example.test/events", backend->lastRequest.url);
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    EXPECT_NE(std::string::npos, backend->lastRequest.body.find("test-game_debug"));
#else
    EXPECT_NE(std::string::npos, backend->lastRequest.body.find("test-game\""));
#endif
    EXPECT_NE(std::string::npos, backend->lastRequest.body.find("\"location\":\"test\""));
    EXPECT_NE(std::string::npos, backend->lastRequest.body.find("\"event\":\"app_open\""));
}

TEST_F(AnalyticsServiceTest, RetriesTemporaryFailureAfterConfiguredDelay) {
    nsAnalyticsService::Shared()->SendEvent("app_open");
    nsAnalyticsService::Shared()->Update(0);
    ASSERT_EQ(1u, backend->sendCount);

    nsHttpResponse failure;
    failure.statusCode = 500;
    failure.diagnostic = "test failure";
    nsHttpClient::Complete(backend->lastRequestId, std::move(failure));
    nsHttpClient::Shared()->Update();

    nsAnalyticsService::Shared()->Update(0.99f);
    EXPECT_EQ(1u, backend->sendCount);
    nsAnalyticsService::Shared()->Update(0.02f);
    EXPECT_EQ(2u, backend->sendCount);
}

TEST_F(AnalyticsServiceTest, KeepsQueueBoundedByDroppingOldestPendingEvent) {
    for (unsigned int i = 0; i < 129; ++i) {
        nsAnalyticsService::Shared()->SendEvent("event_" + std::to_string(i));
    }
    nsAnalyticsService::Shared()->Update(0);
    ASSERT_EQ(1u, backend->sendCount);
    EXPECT_NE(std::string::npos,
              backend->lastRequest.body.find("\"event\":\"event_1\""));
    EXPECT_EQ(std::string::npos,
              backend->lastRequest.body.find("\"event\":\"event_0\""));
}

TEST_F(AnalyticsServiceTest, DisablingAnalyticsClearsQueueAndBlocksNewEvents) {
    nsAnalyticsService::Shared()->SendEvent("queued");
    ASSERT_TRUE(g_cfg->SetValue("anal", 0));
    nsAnalyticsService::Shared()->Update(0);
    ASSERT_TRUE(g_cfg->SetValue("anal", 1));
    nsAnalyticsService::Shared()->Update(0);
    EXPECT_EQ(0u, backend->sendCount);

    nsAnalyticsService::Shared()->SendEvent("new_event");
    nsAnalyticsService::Shared()->Update(0);
    EXPECT_EQ(1u, backend->sendCount);
}

TEST_F(AnalyticsServiceTest, RestoresDisabledStateFromSavedConfig) {
    ASSERT_TRUE(g_cfg->SetValue("anal", 0));
    g_cfg->SaveConfig();
    ASSERT_NE(std::string::npos, g_savedConfig.find("anal \"0"));

    nsAnalyticsService::Release();
    nsHttpClient::Release();
    nsConfig::Release();

    ASSERT_TRUE(nsConfig::Init());
    backend = new FakeBackend();
    ASSERT_TRUE(nsHttpClient::Init(backend));
    ASSERT_TRUE(nsAnalyticsService::Init(ValidConfig()));
    nsAnalyticsService::Shared()->SendEvent("app_open");
    nsAnalyticsService::Shared()->Update(0);
    EXPECT_EQ(0u, backend->sendCount);
}
