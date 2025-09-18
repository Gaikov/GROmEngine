// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLRenderTexturesCache.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "GLRenderTexture.h"

class nsGLRenderTexturesCache {
public:
    nsGLRenderTexture* Create(int width, int height, texfmt_t fmt);
    void Bind(nsGLRenderTexture *t);
    void Release();
    void UnloadFromGPU();

private:
    std::vector<nsGLRenderTexture*> _textures;
    nsGLRenderTexture *_current = nullptr;
};

