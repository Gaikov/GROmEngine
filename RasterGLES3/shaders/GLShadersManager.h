//
// Created by Roman on 08/08/2019.
//

#pragma once

#include "GLProgramsCache.h"
#include "nsLib/factory/ResourcesCache.h"
#include "GLShader.h"

class GLShadersManager : public ResourcesCache<GLShader, int>
{
public:
	nsGLProgramsCache programs;

	GLShadersManager();
	bool Init();
	void Release();
	void Apply(GLShader *s);

	void ApplyTextureParams() const;

protected:
	GLShader *AllocateResource(const char *id, int param) override;
	void FreeResource(GLShader *item) override;

private:
	GLShader _defaultShader;
	GLShader *_boundShader;
};
