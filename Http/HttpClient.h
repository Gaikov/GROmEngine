#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

enum class nsHttpError {
    None,
    Network,
    Timeout,
    Cancelled,
    Unsupported
};

struct nsHttpRequest {
    std::string method = "GET";
    std::string url;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
    unsigned int timeoutMs = 5000;
};

struct nsHttpResponse {
    int statusCode = 0;
    nsHttpError error = nsHttpError::None;
    std::string body;
    std::string diagnostic;
};

class nsHttpClient final {
public:
    using RequestId = std::uint64_t;
    using Callback = std::function<void(const nsHttpResponse &)>;

    class IBackend;

    static bool Init(IBackend *backend = nullptr);
    static void Release();
    static nsHttpClient *Shared();

    RequestId SendAsync(const nsHttpRequest &request, Callback callback);
    void Update();
    void Cancel(RequestId requestId);
    void CancelAll();

    static void Complete(RequestId requestId, nsHttpResponse response);

private:
    struct Completion;

    explicit nsHttpClient(IBackend *backend);
    ~nsHttpClient();

    void QueueCompletion(RequestId requestId, nsHttpResponse response);

    IBackend *_backend = nullptr;
    RequestId _nextRequestId = 1;
    struct Impl;
    Impl *_impl = nullptr;
};
