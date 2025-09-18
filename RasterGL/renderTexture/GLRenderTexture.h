// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLRenderTexture.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "Engine/RenDevice.h"
#include "GLCommon.h"

class nsGLRenderTexture : public IRenderTexture {
friend class nsGLRenderTexturesCache;

public:
    nsGLRenderTexture(int width, int height, texfmt_t fmt);
    ~nsGLRenderTexture() override;
    bool BindTarget();
    bool BindTexture();
    bool IsValid() const { return _fbo != 0; }

private:
    GLuint _fbo = 0;
    GLuint _texture = 0;
    GLuint _zstencil = 0;

    const int _width = 0;
    const int _height = 0;
    const texfmt_t _fmt;

    bool UploadToGPU();
    void UnloadFromGPU();

    void GetSize(int &width, int &height) override;
};
