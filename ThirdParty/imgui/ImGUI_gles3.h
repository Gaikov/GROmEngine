// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImGUI_gles3.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "nsImGUIBackend.h"

class nsImGUI_gles3 final : public nsImGUIBackend {
public:
protected:
    bool BackendInit(void *window) override;
    void BackendShutdown() override;
    void BackendStartFrame() override;
    void BackendRenderDrawData(ImDrawData *drawData) override;

private:
    bool _glfwInitialized = false;
    bool _rendererInitialized = false;
};
