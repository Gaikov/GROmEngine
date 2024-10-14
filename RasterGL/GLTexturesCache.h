//
// Created by Raman Haikou on 2019-07-27.
//

#pragma once
#include "GLCommon.h"
#include "nsLib/factory/ResourcesCache.h"

class GLTexture;

class GLTexturesCache : public ResourcesCache<GLTexture, int>
{
public:
	GLTexturesCache();
	bool BindTexture(GLTexture *t);
	GLTexture *CreateFromBitmapData(nsBitmapData::tSP &data);
    bool HasBoundTexture() { return _current; }

    void UnloadFromGPU();

private:
	GLTexture *_current;
	int       _bitmapId = 0;

private:
	GLTexture *AllocateResource(const char *id, int param) override;
	void FreeResource(GLTexture *item) override;
};