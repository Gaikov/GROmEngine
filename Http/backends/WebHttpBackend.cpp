#include <emscripten/emscripten.h>

#include "Http/HttpBackend.h"

extern "C" EMSCRIPTEN_KEEPALIVE void nsHttpClientWebComplete(
        double requestId, int statusCode, int errorCode,
        const char *body, const char *diagnostic) {
    nsHttpResponse response;
    response.statusCode = statusCode;
    response.error = errorCode == 0 ? nsHttpError::None
            : (errorCode == 2 ? nsHttpError::Timeout : nsHttpError::Network);
    if (body) response.body = body;
    if (diagnostic) response.diagnostic = diagnostic;
    nsHttpClient::Complete(static_cast<nsHttpClient::RequestId>(requestId), std::move(response));
}

EM_JS(void, SendHttpRequest, (double requestId, const char *method, const char *url,
        const char *headersJson, const char *body, int timeoutMs), {
    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), timeoutMs);
    const requestMethod = UTF8ToString(method);
    const requestBody = UTF8ToString(body);
    fetch(UTF8ToString(url), {
        method: requestMethod,
        headers: JSON.parse(UTF8ToString(headersJson)),
        body: requestMethod === 'GET' || requestMethod === 'HEAD' ? undefined : requestBody,
        credentials: 'omit',
        signal: controller.signal
    }).then(async response => {
        const text = (await response.text()).slice(0, 1024 * 1024);
        const bodyPtr = stringToNewUTF8(text);
        _nsHttpClientWebComplete(requestId, response.status, 0, bodyPtr, 0);
        _free(bodyPtr);
    }).catch(error => {
        const diagnostic = stringToNewUTF8(String(error));
        _nsHttpClientWebComplete(requestId, 0, error && error.name === 'AbortError' ? 2 : 1, 0, diagnostic);
        _free(diagnostic);
    }).finally(() => clearTimeout(timer));
});

namespace {
std::string JsonEscape(const std::string &value) {
    std::string result;
    for (const auto ch : value) {
        if (ch == '\\' || ch == '"') result.push_back('\\');
        result.push_back(ch);
    }
    return result;
}

class WebHttpBackend final : public nsHttpClient::IBackend {
public:
    using RequestId = nsHttpClient::RequestId;

    bool Send(RequestId requestId, const nsHttpRequest &request) override {
        std::string headers = "{";
        for (size_t i = 0; i < request.headers.size(); ++i) {
            if (i) headers += ',';
            headers += '"' + JsonEscape(request.headers[i].first) + "\":\""
                    + JsonEscape(request.headers[i].second) + '"';
        }
        headers += '}';
        SendHttpRequest(static_cast<double>(requestId), request.method.c_str(), request.url.c_str(),
                        headers.c_str(), request.body.c_str(), request.timeoutMs);
        return true;
    }

    void CancelAll() override {
    }
};
}

nsHttpClient::IBackend *CreateHttpBackend() {
    return new WebHttpBackend();
}
