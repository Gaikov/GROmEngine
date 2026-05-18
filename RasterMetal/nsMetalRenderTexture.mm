//
// Created by OpenCode
//

#include "nsMetalRenderTexture.h"
#include "nsLib/log.h"

nsMetalRenderTexture::nsMetalRenderTexture(id<MTLDevice> device, int width, int height, texfmt_t fmt)
    : _device(device), _width(width), _height(height), _fmt(fmt) {
}

nsMetalRenderTexture::~nsMetalRenderTexture() {
    ReleaseGPU();
}

bool nsMetalRenderTexture::Upload() {
    if (_colorTexture && _depthStencilTexture) return true;
    ReleaseGPU();
    if (_width <= 0 || _height <= 0) return false;

    MTLTextureDescriptor *colorDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                                         width:_width
                                                                                        height:_height
                                                                                     mipmapped:NO];
    colorDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    _colorTexture = [_device newTextureWithDescriptor:colorDesc];

    MTLTextureDescriptor *depthDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatDepth32Float_Stencil8
                                                                                         width:_width
                                                                                        height:_height
                                                                                     mipmapped:NO];
    depthDesc.usage = MTLTextureUsageRenderTarget;
    _depthStencilTexture = [_device newTextureWithDescriptor:depthDesc];

    if (!_colorTexture || !_depthStencilTexture) {
        Log::Error("Metal: failed to create render texture: %ix%i", _width, _height);
        return false;
    }
    return true;
}

void nsMetalRenderTexture::ReleaseGPU() {
    _colorTexture = nil;
    _depthStencilTexture = nil;
}

void nsMetalRenderTexture::EnsureSize(int width, int height) {
    if (_width == width && _height == height) return;
    _width = width;
    _height = height;
    ReleaseGPU();
}

void nsMetalRenderTexture::GetSize(int &width, int &height) {
    width = _width;
    height = _height;
}

uint64_t nsMetalRenderTexture::GetId() {
    Upload();
    return reinterpret_cast<uint64_t>(_colorTexture);
}

id<MTLTexture> nsMetalRenderTexture::GetColorTexture() {
    Upload();
    return _colorTexture;
}

id<MTLTexture> nsMetalRenderTexture::GetDepthStencilTexture() {
    Upload();
    return _depthStencilTexture;
}

bool nsMetalRenderTexture::Bind(id<MTLRenderCommandEncoder> encoder, int index) {
    if (!Upload()) return false;
    [encoder setFragmentTexture:_colorTexture atIndex:index];
    return true;
}

void nsMetalRenderTexture::Invalidate() {
    ReleaseGPU();
}
