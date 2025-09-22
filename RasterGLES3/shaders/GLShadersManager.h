//
// Created by Roman on 08/08/2019.
//

#pragma once
#include "GLCommon.h"
#include "nsLib/factory/ResourcesCache.h"
#include "GLShader.h"

class GLShadersManager : public ResourcesCache<GLShader, int>
{
public:
	GLShadersManager();
	bool Init();
	void Apply(GLShader *shader);

	void ApplyTextureParams();

protected:
	GLShader *AllocateResource(const char *id, int param) override;
	void FreeResource(GLShader *item) override;

private:
	GLShader _defaultShader;
	GLShader *_boundShader;
};
