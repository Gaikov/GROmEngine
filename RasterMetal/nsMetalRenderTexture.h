//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsMetalTexture.h"

#import <Metal/Metal.h>

class nsMetalRenderTexture final : public IRenderTexture {
public:
    nsMetalRenderTexture(id<MTLDevice> device, int width, int height, texfmt_t fmt);
    ~nsMetalRenderTexture() override;

    void EnsureSize(int width, int height) override;
    void GetSize(int &width, int &height) override;
    uint64_t GetId() override;

    id<MTLTexture> GetColorTexture() const { return _colorTexture; }
    id<MTLTexture> GetDepthStencilTexture() const { return _depthStencilTexture; }
    bool Bind(id<MTLRenderCommandEncoder> encoder, int index);

private:
    id<MTLDevice> _device = nil;
    id<MTLTexture> _colorTexture = nil;
    id<MTLTexture> _depthStencilTexture = nil;
    int _width = 0;
    int _height = 0;
    texfmt_t _fmt = TF_RGBA;

    void Upload();
    void ReleaseGPU();
};
