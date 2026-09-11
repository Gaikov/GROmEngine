//
// Created by OpenCode
//

#include "nsMetalTexturesCache.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"

namespace {
std::string TextureCacheKey(const char *id, int flags) {
    if ((flags & (TLF_PREMULTIPLY_ALPHA | TLF_MIPMAP)) == 0) {
        return id;
    }
    std::string key = id;
    if (flags & TLF_PREMULTIPLY_ALPHA) key += "#pma";
    if (flags & TLF_MIPMAP) key += "#mip";
    return key;
}
}

nsMetalTexturesCache::nsMetalTexturesCache(id<MTLDevice> device, id<MTLCommandQueue> commandQueue)
    : _device(device), _commandQueue(commandQueue) {}

nsMetalTexturesCache::~nsMetalTexturesCache() {
    _current = nullptr;
    ReleaseAll();
}

nsMetalTexture* nsMetalTexturesCache::AllocateResource(const char *id, int param) {
    return nsMetalTexture::Load(_device, _commandQueue, id, param);
}

void nsMetalTexturesCache::FreeResource(nsMetalTexture *item) {
    nsMetalTexture::Free(item);
}

nsMetalTexture* nsMetalTexturesCache::GetTexture(const char *id, int flags) {
    if (!id) {
        return nullptr;
    }

    const auto key = TextureCacheKey(id, flags);
    auto it = _cache.find(key);
    if (it != _cache.end()) {
        Entry &entry = it->second;
        entry.refCount++;
        return entry.item;
    }

    auto item = AllocateResource(id, flags);
    if (item) {
        AddToCache(key.c_str(), item);
    }
    return item;
}

bool nsMetalTexturesCache::BindTexture(nsMetalTexture *t, id<MTLRenderCommandEncoder> encoder) {
    _current = t;
    if (_current) {
        return _current->Bind(encoder, 0);
    }
    if (encoder) {
        [encoder setFragmentTexture:nil atIndex:0];
    }
    return true;
}

void nsMetalTexturesCache::DumpKeys() const {
    Log::Info("...texture cache keys: %i", static_cast<int>(_cache.size()));
    for (const auto &it : _cache) {
        Log::Info("texture key: %s", it.first.c_str());
    }
}

nsMetalTexture* nsMetalTexturesCache::CreateFromBitmapData(nsBitmapData::tSP &data) {
    auto id = StrPrintf("bitmap_texture_%i", _bitmapId);
    _bitmapId++;
    auto t = nsMetalTexture::Create(_device, _commandQueue, id, data);
    if (t) {
        AddToCache(id, t);
    }
    return t;
}

void nsMetalTexturesCache::UnloadFromGPU() const {
    for (const auto &it : _cache) {
        it.second.item->UnloadFromGPU();
    }
}
