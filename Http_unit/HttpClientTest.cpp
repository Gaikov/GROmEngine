#include <thread>

#include <gtest/gtest.h>

#include "Http/HttpBackend.h"

namespace {
class FakeBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        lastRequestId = requestId;
        lastRequest = request;
        return acceptsRequests;
    }

    void CancelAll() override {
        cancelled = true;
    }

    RequestId lastRequestId = 0;
    nsHttpRequest lastRequest;
    bool acceptsRequests = true;
    bool cancelled = false;
};
}

TEST(HttpClient, DeliversCompletionOnlyFromUpdate) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsHttpClient::Init(backend));
    const auto mainThread = std::this_thread::get_id();
    bool called = false;
    std::thread::id callbackThread;
    nsHttpRequest request;
    request.url = "https://example.test/event";
    const auto id = nsHttpClient::Shared()->SendAsync(request, [&](const nsHttpResponse &response) {
        called = true;
        callbackThread = std::this_thread::get_id();
        EXPECT_EQ(202, response.statusCode);
    });
    ASSERT_NE(0u, id);

    std::thread completion([id] {
        nsHttpResponse response;
        response.statusCode = 202;
        nsHttpClient::Complete(id, std::move(response));
    });
    completion.join();
    EXPECT_FALSE(called);
    nsHttpClient::Shared()->Update();
    EXPECT_TRUE(called);
    EXPECT_EQ(mainThread, callbackThread);
    nsHttpClient::Release();
}

TEST(HttpClient, CancelDropsPendingCallback) {
    auto *backend = new FakeBackend();
    ASSERT_TRUE(nsHttpClient::Init(backend));
    bool called = false;
    nsHttpRequest request;
    request.url = "https://example.test/event";
    const auto id = nsHttpClient::Shared()->SendAsync(request, [&](const nsHttpResponse &) {
        called = true;
    });
    nsHttpClient::Shared()->CancelAll();
    nsHttpResponse response;
    response.statusCode = 200;
    nsHttpClient::Complete(id, std::move(response));
    nsHttpClient::Shared()->Update();
    EXPECT_FALSE(called);
    EXPECT_TRUE(backend->cancelled);
    nsHttpClient::Release();
}
