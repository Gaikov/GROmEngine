//
// Created by OpenCode
//
#pragma once

#include <Metal/Metal.h>
#include <string>
#include "Engine/RenDevice.h"
#include "Core/BitmapData.h"

class nsMetalTexture : public ITexture {
public:
    static nsMetalTexture* Load(id<MTLDevice> device, const char *filePath, int flags);
    static void Free(nsMetalTexture *t);

    nsMetalTexture(id<MTLDevice> device, const char *id);
    ~nsMetalTexture() override;

    bool UploadFromBitmap(nsBitmapData *bmData);
    bool Bind(id<MTLRenderCommandEncoder> encoder, int index);

    int GetWidth();
    int GetHeight();
    void GetSize(int &width, int &height) override;
    uint64_t GetId() override;
    const char* GetPath();

private:
    id<MTLDevice>   _device     = nil;
    id<MTLTexture>  _texture    = nil;
    id<MTLSamplerState> _sampler = nil;
    std::string     _id;
    nsBitmapData::tSP _bmData;
    int             _loadFlags  = 0;
};
