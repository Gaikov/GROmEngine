#include <utility>

#include <gtest/gtest.h>

#include "Share/ShareBackend.h"

class Platform;
Platform *App_GetPlatform() {
    return nullptr;
}

namespace {
class FakeBackend final : public nsShareService::IBackend {
public:
    using RequestId = nsShareService::RequestId;

    bool accept = true;
    RequestId lastRequestId = 0;
    nsShareRequest lastRequest;

    bool Share(RequestId requestId, const nsShareRequest &request) override {
        lastRequestId = requestId;
        lastRequest = request;
        return accept;
    }

    void CancelAll() override {
        cancelled = true;
    }

    bool cancelled = false;
};

class ShareServiceTest : public testing::Test {
protected:
    void TearDown() override {
        nsShareService::Release();
    }
};
}

TEST_F(ShareServiceTest, DeliversCompletionFromUpdate) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsShareService::Init(backend));

    bool called = false;
    const auto id = nsShareService::Shared()->ShareAsync(
            {"Title", "Text", "https://example.com", "share.png"},
            [&](const nsShareResult &result) {
                called = true;
                EXPECT_EQ(result.status, nsShareStatus::Completed);
            });
    ASSERT_NE(id, 0u);
    EXPECT_FALSE(called);

    nsShareService::Complete(id, {nsShareStatus::Completed, {}});
    EXPECT_FALSE(called);
    nsShareService::Shared()->Update();
    EXPECT_TRUE(called);
}

TEST_F(ShareServiceTest, AllowsOnlyOneActiveRequest) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsShareService::Init(backend));
    auto callback = [](const nsShareResult &) {};

    const auto first = nsShareService::Shared()->ShareAsync({{}, "One", {}, {}}, callback);
    const auto second = nsShareService::Shared()->ShareAsync({{}, "Two", {}, {}}, callback);
    EXPECT_NE(first, 0u);
    EXPECT_EQ(second, 0u);

    nsShareService::Complete(first, {nsShareStatus::Cancelled, {}});
    nsShareService::Shared()->Update();
    EXPECT_NE(nsShareService::Shared()->ShareAsync({{}, "Two", {}, {}}, callback), 0u);
}

TEST_F(ShareServiceTest, RejectsEmptyOrBackendFailure) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsShareService::Init(backend));
    auto callback = [](const nsShareResult &) {};

    EXPECT_EQ(nsShareService::Shared()->ShareAsync({}, callback), 0u);
    backend->accept = false;
    EXPECT_EQ(nsShareService::Shared()->ShareAsync({{}, "Text", {}, {}}, callback), 0u);
}

TEST_F(ShareServiceTest, IgnoresLateCompletionAfterRelease) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsShareService::Init(backend));
    bool called = false;
    const auto id = nsShareService::Shared()->ShareAsync(
            {{}, "Text", {}, {}}, [&](const nsShareResult &) { called = true; });
    nsShareService::Release();
    nsShareService::Complete(id, {nsShareStatus::Completed, {}});
    EXPECT_FALSE(called);
}
