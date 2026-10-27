//
// Created by OpenCode
//
#pragma once

#include "nsMetalTexture.h"
#include "nsLib/factory/ResourcesCache.h"
#include <map>
#include <string>

class nsMetalTexturesCache : public ResourcesCache<nsMetalTexture, int> {
public:
    nsMetalTexturesCache(id<MTLDevice> device, id<MTLCommandQueue> commandQueue);
    ~nsMetalTexturesCache();

    nsMetalTexture* GetTexture(const char *id, int flags);
    nsMetalTexture* CreateFromBitmapData(nsBitmapData::tSP &data);

    bool BindTexture(nsMetalTexture *t, id<MTLRenderCommandEncoder> encoder, unsigned slot = 0);
    nsMetalTexture* GetBoundTexture(unsigned slot = 0) const { return _current[slot]; }
    bool HasBoundTexture(unsigned slot = 0) const { return _current[slot] != nullptr; }

    void DumpKeys() const;
    void UnloadFromGPU() const;

private:
    id<MTLDevice>       _device     = nil;
    id<MTLCommandQueue> _commandQueue = nil;
    nsMetalTexture     *_current[2] = {};
    int                 _bitmapId   = 0;

    nsMetalTexture* AllocateResource(const char *id, int param) override;
    void FreeResource(nsMetalTexture *item) override;
};
