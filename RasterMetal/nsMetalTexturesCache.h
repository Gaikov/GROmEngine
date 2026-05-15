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
    nsMetalTexturesCache(id<MTLDevice> device);
    ~nsMetalTexturesCache();

    nsMetalTexture* GetTexture(const char *id, int flags);

    bool BindTexture(nsMetalTexture *t, id<MTLRenderCommandEncoder> encoder);
    nsMetalTexture* GetBoundTexture() { return _current; }
    bool HasBoundTexture() const { return _current != nullptr; }

    void DumpKeys() const;

private:
    id<MTLDevice>       _device     = nil;
    nsMetalTexture     *_current    = nullptr;

    nsMetalTexture* AllocateResource(const char *id, int param) override;
    void FreeResource(nsMetalTexture *item) override;
};
