#pragma once

#include "ShareService.h"

class nsShareService::IBackend {
public:
    virtual ~IBackend() = default;
    virtual bool Share(RequestId requestId, const nsShareRequest &request) = 0;
    virtual void CancelAll() = 0;
};

nsShareService::IBackend *CreateShareBackend();
