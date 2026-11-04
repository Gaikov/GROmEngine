// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLRenderTexture.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "GLBaseTexture.h"
#include "Engine/RenDevice.h"
#include "GLCommon.h"

class nsGLRenderTexture : virtual public nsGLBaseTexture, virtual public IRenderTexture {
friend class nsGLRenderTexturesCache;

public:
    static bool InitGLExtensionsFBO();
    static void Unbind();

    nsGLRenderTexture(int width, int height, texfmt_t fmt);
    ~nsGLRenderTexture() override;
    bool BindTarget();
    bool Bind() override;
    bool IsValid() const { return _fbo != 0; }
    void EnsureSize(int width, int height) override;
    ITexture *GetDepthTexture() override { return &_depthView; }
    uint64_t GetId() override { return _texture; }
    bool IsMipmapped() const override { return false; }

private:
    class nsDepthView final : public nsGLBaseTexture {
    public:
        explicit nsDepthView(nsGLRenderTexture &owner) : _owner(owner) {}
        void GetSize(int &width, int &height) override { _owner.GetSize(width, height); }
        uint64_t GetId() override { return _owner._zstencil; }
        bool Bind() override;
        bool IsMipmapped() const override { return false; }
        void Invalidate() { ++_bindingVersion; }
    private:
        nsGLRenderTexture &_owner;
    };
    nsDepthView _depthView{*this};
    GLuint _fbo = 0;
    GLuint _texture = 0;
    GLuint _zstencil = 0;

    int _width = 0;
    int _height = 0;
    const texfmt_t _fmt;

    bool UploadToGPU();
    void UnloadFromGPU();

    void GetSize(int &width, int &height) override;
};
