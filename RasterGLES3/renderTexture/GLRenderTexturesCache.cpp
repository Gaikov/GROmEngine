//
// Created by Roman on 9/18/2025.
//

#include "GLRenderTexturesCache.h"

bool nsGLRenderTexturesCache::Init() {
    _hasFeature = nsGLRenderTexture::InitGLExtensionsFBO();
    return _hasFeature;
}

nsGLRenderTexture * nsGLRenderTexturesCache::Create(const int width, const int height, const texfmt_t fmt) {
    if (!_hasFeature) {
        Log::Warning("Render textures are not supported by the device");
        return nullptr;
    }

    const auto t = new nsGLRenderTexture(width, height, fmt);
    if (t->IsValid()) {
        _textures.push_back(t);
        return t;
    }

    delete t;
    return nullptr;
}

void nsGLRenderTexturesCache::Bind(nsGLRenderTexture *t) {
    if (t != _current) {
        _current = t;
        if (_current) {
            _current->BindTarget();
        }
        else {
            nsGLRenderTexture::Unbind();
        }
    }
}

void nsGLRenderTexturesCache::Release() {
    for (const auto t : _textures) {
        delete t;
    }
    _textures.clear();
}

void nsGLRenderTexturesCache::UnloadFromGPU() {
    for (const auto t : _textures) {
        t->UnloadFromGPU();
    }
    _current = nullptr;
}
