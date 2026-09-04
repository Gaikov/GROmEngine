#include <thread>

#include <curl/curl.h>

#include "Http/HttpBackend.h"

namespace {
constexpr size_t MAX_RESPONSE_SIZE = 1024 * 1024;

size_t WriteBody(char *data, size_t size, size_t count, void *userData) {
    auto &body = *static_cast<std::string *>(userData);
    const auto bytes = size * count;
    if (body.size() + bytes > MAX_RESPONSE_SIZE) {
        return 0;
    }
    body.append(data, bytes);
    return bytes;
}

class CurlHttpBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    CurlHttpBackend() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~CurlHttpBackend() override = default;

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        std::thread([requestId, request] {
            nsHttpResponse response;
            auto *curl = curl_easy_init();
            if (!curl) {
                response.error = nsHttpError::Unsupported;
                response.diagnostic = "curl_easy_init failed";
                nsHttpClient::Complete(requestId, std::move(response));
                return;
            }

            curl_slist *headers = nullptr;
            for (const auto &[name, value] : request.headers) {
                headers = curl_slist_append(headers, (name + ": " + value).c_str());
            }
            curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.method.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            if (!request.body.empty() && request.method != "GET" && request.method != "HEAD") {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.body.data());
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(request.body.size()));
            }
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(request.timeoutMs));
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBody);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

            const auto result = curl_easy_perform(curl);
            long statusCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);
            response.statusCode = static_cast<int>(statusCode);
            if (result != CURLE_OK) {
                response.error = result == CURLE_OPERATION_TIMEDOUT
                        ? nsHttpError::Timeout
                        : nsHttpError::Network;
                response.diagnostic = curl_easy_strerror(result);
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            nsHttpClient::Complete(requestId, std::move(response));
        }).detach();
        return true;
    }

    void CancelAll() override {
        // Detached requests may finish at the OS level; nsHttpClient drops late completions.
    }
};
}

nsHttpClient::IBackend *CreateHttpBackend() {
    return new CurlHttpBackend();
}
