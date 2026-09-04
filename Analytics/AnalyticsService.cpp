#include <utility>

#include "AnalyticsService.h"
#include "AnalyticsEvent.h"
#include "Core/Config.h"
#include "Engine/Platform.h"
#include "nsLib/log.h"

namespace {
constexpr size_t MAX_QUEUE_SIZE = 128;
constexpr unsigned int MAX_RETRIES = 3;
constexpr float RETRY_DELAYS[MAX_RETRIES] = {1.0f, 5.0f, 30.0f};

bool IsDebugBuild() {
#if defined(DEBUG) || defined(_DEBUG) || !defined(NDEBUG)
    return true;
#else
    return false;
#endif
}

std::string PlatformLocation() {
#if defined(__ANDROID__)
    return "android";
#elif defined(WEB_ASM)
    const auto *domain = App_GetPlatform()->GetDomainName();
    return domain && domain[0] ? domain : "web";
#elif defined(_WIN32)
    return "windows";
#elif defined(__APPLE__)
    return "macos";
#elif defined(__linux__)
    return "linux";
#else
    return "unknown";
#endif
}

bool IsSuccess(const nsHttpResponse &response) {
    return response.error == nsHttpError::None
            && response.statusCode >= 200 && response.statusCode < 300;
}

bool IsRetriable(const nsHttpResponse &response) {
    if (response.error == nsHttpError::Network || response.error == nsHttpError::Timeout) {
        return true;
    }
    return response.statusCode == 408 || response.statusCode == 429
            || response.statusCode >= 500;
}
}

nsAnalyticsService *nsAnalyticsService::_instance = nullptr;
nsVar *nsAnalyticsService::_enabled = nullptr;

bool nsAnalyticsService::Init(const nsAnalyticsConfig &config) {
    if (_instance) return true;
    if (config.endpointUrl.empty() || config.appId.empty()) {
        Log::Error("Analytics endpoint URL and app ID are required");
        return false;
    }
    if (!g_cfg) {
        Log::Error("Analytics requires initialized config");
        return false;
    }

    _enabled = g_cfg->RegVar("anal", "1", GVF_SAVABLE);
    if (!_enabled) {
        Log::Error("Unable to register analytics console variable");
        return false;
    }
    _instance = new nsAnalyticsService(config);
    return true;
}

void nsAnalyticsService::Release() {
    delete _instance;
    _instance = nullptr;
    _enabled = nullptr;
}

nsAnalyticsService *nsAnalyticsService::Shared() {
    return _instance;
}

nsAnalyticsService::nsAnalyticsService(nsAnalyticsConfig config)
        : _config(std::move(config)), _wasEnabled(Enabled()) {
    _config.appId = nsAnalyticsEvent::BuildAppId(
            _config.appId, _config.debugAppIdSuffix, IsDebugBuild());
    if (_config.location.empty()) {
        _config.location = PlatformLocation();
    }
}

nsAnalyticsService::~nsAnalyticsService() {
    Clear();
}

void nsAnalyticsService::SendEvent(const std::string &action, const Params &params) {
    if (!Enabled()) return;
    auto value = nsAnalyticsEvent::Format(action, params);
    if (!nsAnalyticsEvent::IsValid(value)) {
        Log::Warning("Analytics event rejected: encoded length is %u (max %u)",
                     static_cast<unsigned int>(value.size()),
                     static_cast<unsigned int>(nsAnalyticsEvent::MAX_ENCODED_LENGTH));
        return;
    }
    if (_queue.size() >= MAX_QUEUE_SIZE) {
        Log::Warning("Analytics queue is full; dropping the oldest event");
        if (_requestActive && _queue.size() > 1) {
            _queue.erase(_queue.begin() + 1);
        } else if (!_requestActive) {
            _queue.pop_front();
        } else {
            return;
        }
    }
    _queue.push_back({std::move(value)});
}

void nsAnalyticsService::Update(float frameTime) {
    const bool isEnabled = Enabled();
    if (!isEnabled) {
        if (_wasEnabled || !_queue.empty() || _requestActive) Clear();
        _wasEnabled = false;
        return;
    }
    _wasEnabled = true;
    if (_requestActive || _queue.empty()) return;
    if (_queue.front().retryDelay > 0) {
        _queue.front().retryDelay -= frameTime;
        if (_queue.front().retryDelay > 0) return;
    }
    StartRequest();
}

void nsAnalyticsService::StartRequest() {
    if (!Enabled() || _queue.empty()) return;
    auto *http = nsHttpClient::Shared();
    if (!http) {
        Log::Warning("Analytics HTTP client is unavailable");
        _queue.pop_front();
        return;
    }

    nsHttpRequest request;
    request.method = "POST";
    request.url = _config.endpointUrl;
    request.headers.emplace_back("Content-Type", "application/json");
    request.timeoutMs = 5000;
    request.body = nsAnalyticsEvent::BuildPayload(
            _config.appId, _config.location, _queue.front().value);

    const auto requestId = http->SendAsync(request, [this](const nsHttpResponse &response) {
        HandleResponse(response);
    });
    if (!requestId) {
        nsHttpResponse response;
        response.error = nsHttpError::Unsupported;
        response.diagnostic = "HTTP backend rejected request";
        HandleResponse(response);
        return;
    }
    _activeRequestId = requestId;
    _requestActive = true;
}

void nsAnalyticsService::HandleResponse(const nsHttpResponse &response) {
    _activeRequestId = 0;
    _requestActive = false;
    if (_queue.empty() || !Enabled()) return;
    if (IsSuccess(response)) {
        _queue.pop_front();
        return;
    }
    auto &event = _queue.front();
    if (IsRetriable(response) && event.retryCount < MAX_RETRIES) {
        event.retryDelay = RETRY_DELAYS[event.retryCount++];
        Log::Warning("Analytics request failed; retry %u scheduled: %s",
                     event.retryCount, response.diagnostic.c_str());
        return;
    }
    Log::Warning("Analytics event dropped after HTTP status %i: %s",
                 response.statusCode, response.diagnostic.c_str());
    _queue.pop_front();
}

void nsAnalyticsService::Clear() {
    if (_activeRequestId) {
        if (auto *http = nsHttpClient::Shared()) http->Cancel(_activeRequestId);
    }
    _queue.clear();
    _activeRequestId = 0;
    _requestActive = false;
}

bool nsAnalyticsService::Enabled() const {
    return _enabled && _enabled->Bool();
}
