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
	GLTexture *GetTexture(const char *id, int flags);
	bool BindTexture(nsGLBaseTexture *t, unsigned slot = 0);
    void ApplyBindings();
	GLTexture *CreateFromBitmapData(nsBitmapData::tSP &data);
    void DumpKeys() const;

	[[nodiscard]] bool HasBoundTexture(unsigned slot = 0) const { return _current[slot] != nullptr; }
	[[nodiscard]] nsGLBaseTexture *GetBoundTexture(unsigned slot = 0) const { return _current[slot]; }
	[[nodiscard]] bool HasBoundMipmappedTexture(unsigned slot = 0) const { return _current[slot] && _current[slot]->IsMipmapped(); }

    void UnloadFromGPU();

private:
	nsGLBaseTexture *_current[2] = {};
    unsigned _boundVersions[2] = {};
    bool _bindingsValid[2] = {};
	int					_bitmapId = 0;

private:
	GLTexture *AllocateResource(const char *id, int param) override;
	void FreeResource(GLTexture *item) override;
};
