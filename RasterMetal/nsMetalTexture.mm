//
// Created by OpenCode
//

#include "nsMetalTexture.h"
#include "ImageDecoder/BitmapLoader.h"
#include "nsLib/log.h"

nsMetalTexture::nsMetalTexture(id<MTLDevice> device, const char *id)
    : _device(device), _id(id) {}

nsMetalTexture::~nsMetalTexture() {
    _texture = nil;
    _sampler = nil;
}

nsMetalTexture* nsMetalTexture::Load(id<MTLDevice> device, const char *filePath, int flags) {
    Log::Info("...loading bitmap data: %s", filePath);

    const auto bmData = BitmapLoader::LoadFromFile(filePath);
    if (!bmData) {
        Log::Warning("can't load texture: %s", filePath);
        return nullptr;
    }

    auto texture = new nsMetalTexture(device, filePath);
    texture->_bmData = bmData;
    texture->_loadFlags = flags;
    if (!texture->UploadFromBitmap(bmData.get())) {
        delete texture;
        return nullptr;
    }

    MTLSamplerDescriptor *samplerDesc = [MTLSamplerDescriptor new];
    samplerDesc.minFilter = MTLSamplerMinMagFilterLinear;
    samplerDesc.magFilter = MTLSamplerMinMagFilterLinear;
    samplerDesc.sAddressMode = MTLSamplerAddressModeRepeat;
    samplerDesc.tAddressMode = MTLSamplerAddressModeRepeat;
    texture->_sampler = [device newSamplerStateWithDescriptor:samplerDesc];

    return texture;
}

void nsMetalTexture::Free(nsMetalTexture *t) {
    delete t;
}

bool nsMetalTexture::UploadFromBitmap(nsBitmapData *bmData) {
    if (!bmData) return false;

    MTLTextureDescriptor *desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                    width:bmData->GetWidth()
                                                                                   height:bmData->GetHeight()
                                                                                mipmapped:NO];
    _texture = [_device newTextureWithDescriptor:desc];
    if (!_texture) {
        Log::Error("Metal: failed to create texture: %s", _id.c_str());
        return false;
    }

    MTLRegion region = MTLRegionMake2D(0, 0, bmData->GetWidth(), bmData->GetHeight());
    [_texture replaceRegion:region
                mipmapLevel:0
                  withBytes:bmData->GetData()
                bytesPerRow:bmData->GetWidth() * 4];

    return true;
}

bool nsMetalTexture::Bind(id<MTLRenderCommandEncoder> encoder, int index) {
    if (!_texture) return false;
    [encoder setFragmentTexture:_texture atIndex:index];
    if (_sampler) {
        [encoder setFragmentSamplerState:_sampler atIndex:index];
    }
    return true;
}

int nsMetalTexture::GetWidth() {
    return _bmData ? _bmData->GetWidth() : 1;
}

int nsMetalTexture::GetHeight() {
    return _bmData ? _bmData->GetHeight() : 1;
}

void nsMetalTexture::GetSize(int &width, int &height) {
    width = GetWidth();
    height = GetHeight();
}

uint64_t nsMetalTexture::GetId() {
    return reinterpret_cast<uint64_t>(_texture);
}

const char* nsMetalTexture::GetPath() {
    return _id.c_str();
}
