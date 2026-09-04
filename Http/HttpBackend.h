#pragma once

#include "HttpClient.h"

class nsHttpClient::IBackend {
public:
    virtual ~IBackend() = default;
    virtual bool Send(RequestId requestId, const nsHttpRequest &request) = 0;
    virtual void CancelAll() = 0;
};

nsHttpClient::IBackend *CreateHttpBackend();
