//
// Created by Raman Haikou on 2019-07-27.
//

#include "GLTexturesCache.h"
#include "GLTexture.h"

GLTexturesCache::GLTexturesCache() :
		_current(nullptr)
{
}

GLTexture *GLTexturesCache::AllocateResource(const char *id, int param)
{
	return GLTexture::Load(id);
}

void GLTexturesCache::FreeResource(GLTexture *item)
{
	GLTexture::Free(item);
}

bool GLTexturesCache::BindTexture(GLTexture *t)
{
	if (_current != t)
	{
		_current = t;
		if (_current)
		{
			glEnable(GL_TEXTURE_2D);
			_current->Bind();

		} else
		{
			glDisable(GL_TEXTURE_2D);
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

void GLTexturesCache::UnloadFromGPU() {
    for (const auto& it : _cache) {
        auto t = it.second.item;
        t->UnloadFromGPU();
    }
}

