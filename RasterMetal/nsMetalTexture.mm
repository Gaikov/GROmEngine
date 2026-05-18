//
// Created by OpenCode
//

#include "nsMetalTexture.h"
#include "ImageDecoder/BitmapLoader.h"
#include "nsLib/log.h"

nsMetalTexture::nsMetalTexture(id<MTLDevice> device, const char *id)
    : _device(device), _id(id) {}

nsMetalTexture::~nsMetalTexture() {
    UnloadFromGPU();
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
    if ((flags & TLF_PREMULTIPLY_ALPHA) != 0) {
        texture->_bmData->PremultiplyAlpha();
    }

    return texture;
}

nsMetalTexture* nsMetalTexture::Create(id<MTLDevice> device, const char *id, nsBitmapData::tSP data) {
    if (!data) return nullptr;
    auto texture = new nsMetalTexture(device, id);
    texture->_bmData = std::move(data);
    return texture;
}

void nsMetalTexture::Free(nsMetalTexture *t) {
    delete t;
}

bool nsMetalTexture::UploadFromBitmap(nsBitmapData *bmData) {
    if (!bmData) return false;
    if (_texture) return true;

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

bool nsMetalTexture::UploadToGPU() {
    return _texture || UploadFromBitmap(_bmData.get());
}

bool nsMetalTexture::Bind(id<MTLRenderCommandEncoder> encoder, int index) {
    if (!UploadToGPU()) return false;
    [encoder setFragmentTexture:_texture atIndex:index];
    return true;
}

void nsMetalTexture::UnloadFromGPU() {
    _texture = nil;
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
    UploadToGPU();
    return reinterpret_cast<uint64_t>(_texture);
}

const char* nsMetalTexture::GetPath() {
    return _id.c_str();
}
