#include <map>
#include <mutex>
#include <utility>
#include <vector>

#include "HttpBackend.h"
#include "nsLib/log.h"

namespace {
std::mutex g_clientMutex;
nsHttpClient *g_client = nullptr;
}

struct nsHttpClient::Completion {
    RequestId requestId = 0;
    nsHttpResponse response;
};

struct nsHttpClient::Impl {
    std::mutex completionMutex;
    std::vector<Completion> completions;
    std::map<RequestId, Callback> callbacks;
};

bool nsHttpClient::Init(IBackend *backend) {
    std::lock_guard lock(g_clientMutex);
    if (g_client) {
        return true;
    }
    if (!backend) backend = CreateHttpBackend();
    if (!backend) {
        Log::Error("Unable to create HTTP backend");
        return false;
    }
    g_client = new nsHttpClient(backend);
    return true;
}

void nsHttpClient::Release() {
    nsHttpClient *client = nullptr;
    {
        std::lock_guard lock(g_clientMutex);
        client = g_client;
        g_client = nullptr;
    }
    delete client;
}

nsHttpClient *nsHttpClient::Shared() {
    std::lock_guard lock(g_clientMutex);
    return g_client;
}

nsHttpClient::nsHttpClient(IBackend *backend) : _backend(backend), _impl(new Impl()) {
}

nsHttpClient::~nsHttpClient() {
    CancelAll();
    delete _backend;
    delete _impl;
}

nsHttpClient::RequestId nsHttpClient::SendAsync(
        const nsHttpRequest &request, Callback callback) {
    if (request.url.empty() || !_backend) {
        return 0;
    }

    const auto requestId = _nextRequestId++;
    if (callback) {
        _impl->callbacks.emplace(requestId, std::move(callback));
    }
    if (!_backend->Send(requestId, request)) {
        _impl->callbacks.erase(requestId);
        return 0;
    }
    return requestId;
}

void nsHttpClient::Update() {
    std::vector<Completion> completions;
    {
        std::lock_guard lock(_impl->completionMutex);
        completions.swap(_impl->completions);
    }

    for (auto &completion : completions) {
        const auto found = _impl->callbacks.find(completion.requestId);
        if (found == _impl->callbacks.end()) {
            continue;
        }
        auto callback = std::move(found->second);
        _impl->callbacks.erase(found);
        callback(completion.response);
    }
}

void nsHttpClient::Cancel(RequestId requestId) {
    _impl->callbacks.erase(requestId);
}

void nsHttpClient::CancelAll() {
    if (_backend) {
        _backend->CancelAll();
    }
    _impl->callbacks.clear();
    std::lock_guard lock(_impl->completionMutex);
    _impl->completions.clear();
}

void nsHttpClient::Complete(RequestId requestId, nsHttpResponse response) {
    std::lock_guard lock(g_clientMutex);
    if (g_client) {
        g_client->QueueCompletion(requestId, std::move(response));
    }
}

void nsHttpClient::QueueCompletion(RequestId requestId, nsHttpResponse response) {
    std::lock_guard lock(_impl->completionMutex);
    _impl->completions.push_back({requestId, std::move(response)});
}
