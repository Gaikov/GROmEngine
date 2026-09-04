#pragma once

#include <cstdint>
#include <functional>
#include <string>

struct nsShareRequest {
    std::string title;
    std::string text;
    std::string url;
    std::string imageAssetPath;
};

enum class nsShareStatus {
    Presented,
    Completed,
    Copied,
    Cancelled,
    Unsupported,
    Error
};

struct nsShareResult {
    nsShareStatus status = nsShareStatus::Error;
    std::string diagnostic;
};

class nsShareService final {
public:
    using RequestId = std::uint64_t;
    using Callback = std::function<void(const nsShareResult &)>;

    class IBackend;

    static bool Init(IBackend *backend = nullptr);
    static void Release();
    static nsShareService *Shared();

    RequestId ShareAsync(const nsShareRequest &request, Callback callback);
    void Update();

    static void Complete(RequestId requestId, nsShareResult result);

private:
    struct Completion;
    struct Impl;

    explicit nsShareService(IBackend *backend);
    ~nsShareService();

    void QueueCompletion(RequestId requestId, nsShareResult result);

    IBackend *_backend = nullptr;
    RequestId _nextRequestId = 1;
    Impl *_impl = nullptr;
};
