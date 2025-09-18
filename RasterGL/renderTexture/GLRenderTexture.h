// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLRenderTexture.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/RenDevice.h"
#include "GLCommon.h"

class nsGLRenderTexture : public IRenderTexture {
public:
    nsGLRenderTexture(int width, int height);
    bool BindTarget();
    bool BindTexture();

private:
    GLuint _fbo = 0;
    GLuint _texture = 0;
    GLuint _zstencil = 0;

    int _width = 0;
    int _height = 0;

    bool UploadToGPU();
    void UnloadFromGPU();

    void GetSize(int &width, int &height) override;
};
