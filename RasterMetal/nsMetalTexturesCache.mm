//
// Created by OpenCode
//

#include "nsMetalTexturesCache.h"
#include "nsLib/log.h"

nsMetalTexturesCache::nsMetalTexturesCache(id<MTLDevice> device)
    : _device(device) {}

nsMetalTexturesCache::~nsMetalTexturesCache() = default;

nsMetalTexture* nsMetalTexturesCache::AllocateResource(const char *id, int param) {
    return nsMetalTexture::Load(_device, id, param);
}

void nsMetalTexturesCache::FreeResource(nsMetalTexture *item) {
    nsMetalTexture::Free(item);
}

nsMetalTexture* nsMetalTexturesCache::GetTexture(const char *id, int flags) {
    return GetResource(id, flags);
}

bool nsMetalTexturesCache::BindTexture(nsMetalTexture *t, id<MTLRenderCommandEncoder> encoder) {
    if (_current != t) {
        _current = t;
        if (_current) {
            return _current->Bind(encoder, 0);
        }
        return true;
    }
    return false;
}

void nsMetalTexturesCache::DumpKeys() const {
    Log::Info("...Metal texture cache not dumpable in release");
}
