//
// Created by Raman Haikou on 2019-07-27.
//

#ifndef _GROM_GLTEXTURESCACHE_H_
#define _GROM_GLTEXTURESCACHE_H_

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
	int       _bitmapId;

private:
	GLTexture *AllocateResource(const char *id, int param) override;
	void FreeResource(GLTexture *item) override;
};


#endif //_GROM_GLTEXTURESCACHE_H_
