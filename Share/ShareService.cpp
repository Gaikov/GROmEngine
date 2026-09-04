#include <mutex>
#include <utility>
#include <vector>

#include "ShareBackend.h"
#include "nsLib/log.h"

namespace {
std::mutex g_serviceMutex;
nsShareService *g_service = nullptr;
}

struct nsShareService::Completion {
    RequestId requestId = 0;
    nsShareResult result;
};

struct nsShareService::Impl {
    std::mutex completionMutex;
    std::vector<Completion> completions;
    RequestId activeRequestId = 0;
    Callback callback;
};

bool nsShareService::Init(IBackend *backend) {
    std::lock_guard lock(g_serviceMutex);
    if (g_service) return true;
    if (!backend) backend = CreateShareBackend();
    if (!backend) {
        Log::Error("Unable to create share backend");
        return false;
    }
    g_service = new nsShareService(backend);
    return true;
}

void nsShareService::Release() {
    nsShareService *service = nullptr;
    {
        std::lock_guard lock(g_serviceMutex);
        service = g_service;
        g_service = nullptr;
    }
    delete service;
}

nsShareService *nsShareService::Shared() {
    std::lock_guard lock(g_serviceMutex);
    return g_service;
}

nsShareService::nsShareService(IBackend *backend)
    : _backend(backend), _impl(new Impl()) {
}

nsShareService::~nsShareService() {
    if (_backend) _backend->CancelAll();
    delete _backend;
    delete _impl;
}

nsShareService::RequestId nsShareService::ShareAsync(
        const nsShareRequest &request, Callback callback) {
    if (!_backend || _impl->activeRequestId || !callback
            || (request.text.empty() && request.url.empty())) {
        return 0;
    }

    const auto requestId = _nextRequestId++;
    _impl->activeRequestId = requestId;
    _impl->callback = std::move(callback);
    if (!_backend->Share(requestId, request)) {
        _impl->activeRequestId = 0;
        _impl->callback = {};
        return 0;
    }
    return requestId;
}

void nsShareService::Update() {
    std::vector<Completion> completions;
    {
        std::lock_guard lock(_impl->completionMutex);
        completions.swap(_impl->completions);
    }

    for (auto &completion : completions) {
        if (completion.requestId != _impl->activeRequestId || !_impl->callback) continue;
        auto callback = std::move(_impl->callback);
        _impl->activeRequestId = 0;
        callback(completion.result);
    }
}

void nsShareService::Complete(RequestId requestId, nsShareResult result) {
    std::lock_guard lock(g_serviceMutex);
    if (g_service) g_service->QueueCompletion(requestId, std::move(result));
}

void nsShareService::QueueCompletion(RequestId requestId, nsShareResult result) {
    std::lock_guard lock(_impl->completionMutex);
    _impl->completions.push_back({requestId, std::move(result)});
}
