//
// Created by Raman Haikou on 2019-07-27.
//

#include "GLTexturesCache.h"
#include "GLTexture.h"
#include <string>

namespace {
std::string TextureCacheKey(const char *id, int flags) {
	if ((flags & TLF_PREMULTIPLY_ALPHA) == 0) {
		return id;
	}
	std::string key = id;
	key += "#pma";
	return key;
}
}

GLTexturesCache::GLTexturesCache() :
		_current(nullptr)
{
}

GLTexture *GLTexturesCache::AllocateResource(const char *id, int param)
{
	return GLTexture::Load(id, param);
}

void GLTexturesCache::FreeResource(GLTexture *item)
{
	GLTexture::Free(item);
}

GLTexture *GLTexturesCache::GetTexture(const char *id, int flags) {
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

bool GLTexturesCache::BindTexture(nsGLBaseTexture *t)
{
	if (_current != t)
	{
		_current = t;
		if (_current)
		{
			_current->Bind();
		}
        return true;
	}
	return false;
}

GLTexture *GLTexturesCache::CreateFromBitmapData(nsBitmapData::tSP &data)
{
	auto id = StrPrintf("bitmap_texture_%i", _bitmapId);
	_bitmapId ++;
	auto t = GLTexture::Create(id, data);
	AddToCache(id, t);
	return t;
}

void GLTexturesCache::DumpKeys() const {
    Log::Info("...texture cache keys: %i", static_cast<int>(_cache.size()));
    for (const auto &it : _cache) {
        Log::Info("texture key: %s", it.first.c_str());
    }
}

void GLTexturesCache::UnloadFromGPU() const {
    for (const auto& it : _cache) {
        auto t = it.second.item;
        t->UnloadFromGPU();
    }
}

