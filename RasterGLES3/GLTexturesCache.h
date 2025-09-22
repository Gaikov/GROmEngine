//
// Created by Raman Haikou on 2019-07-27.
//

#pragma once
#include "GLBaseTexture.h"
#include "GLCommon.h"
#include "nsLib/factory/ResourcesCache.h"

class GLTexture;

class GLTexturesCache : public ResourcesCache<GLTexture, int>
{
public:
	GLTexturesCache();
	bool BindTexture(nsGLBaseTexture *t);
	GLTexture *CreateFromBitmapData(nsBitmapData::tSP &data);

	[[nodiscard]] bool HasBoundTexture() const { return _current; }
	[[nodiscard]] nsGLBaseTexture *GetBoundTexture() const { return _current; }

    void UnloadFromGPU() const;

private:
	nsGLBaseTexture		*_current;
	int					_bitmapId = 0;

private:
	GLTexture *AllocateResource(const char *id, int param) override;
	void FreeResource(GLTexture *item) override;
};