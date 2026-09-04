#pragma once

#include <deque>
#include <map>
#include <string>

#include "Http/HttpClient.h"

class nsVar;

struct nsAnalyticsConfig {
    std::string endpointUrl;
    std::string appId;
    std::string debugAppIdSuffix;
    std::string location;
};

class nsAnalyticsService final {
public:
    using Params = std::map<std::string, std::string>;

    static bool Init(const nsAnalyticsConfig &config);
    static void Release();
    static nsAnalyticsService *Shared();

    void SendEvent(const std::string &action, const Params &params = {});
    void Update(float frameTime);

private:
    struct QueuedEvent {
        std::string value;
        unsigned int retryCount = 0;
        float retryDelay = 0;
    };

    explicit nsAnalyticsService(nsAnalyticsConfig config);
    ~nsAnalyticsService();

    void StartRequest();
    void HandleResponse(const nsHttpResponse &response);
    void Clear();
    bool Enabled() const;

    static nsAnalyticsService *_instance;
    static nsVar *_enabled;

    nsAnalyticsConfig _config;
    std::deque<QueuedEvent> _queue;
    nsHttpClient::RequestId _activeRequestId = 0;
    bool _requestActive = false;
    bool _wasEnabled = true;
};
