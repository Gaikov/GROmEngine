// Copyright (c) 2003-2026, Roman Gaikov. All rights reserved.
#pragma once

#include "nsImGUIBackend.h"

class nsMetalRenderDevice;

class nsImGUI_metal final : public nsImGUIBackend {
protected:
    bool BackendInit(void *window) override;
    void BackendShutdown() override;
    void BackendStartFrame() override;
    void BackendRenderDrawData(ImDrawData *drawData) override;

private:
    nsMetalRenderDevice *_renderDevice = nullptr;
    bool _glfwInitialized = false;
    bool _rendererInitialized = false;
    bool _frameReady = false;
};
