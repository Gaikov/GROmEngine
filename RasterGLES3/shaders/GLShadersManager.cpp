//
// Created by Roman on 08/08/2019.
//

#include "GLShadersManager.h"

GLShadersManager::GLShadersManager() :
		_boundShader(nullptr)
{
}

GLShader *GLShadersManager::AllocateResource(const char *id, int param)
{
    if (!StrCheck(id)) {
        return nullptr;
    }

	auto shader = new GLShader();
	shader->m_fileName = id;
	shader->Reload();
	return shader;
}

void GLShadersManager::FreeResource(GLShader *item)
{
	delete item;
}

bool GLShadersManager::Init()
{
	if (!programs.Init()) {
		return false;
	}
	Apply(&_defaultShader);
	return true;
}

void GLShadersManager::Release() {
	Log::Info("...releasing shaders");
	ReleaseAll();

	programs.Release();
}

void GLShadersManager::Apply(GLShader *s)
{
	if (!s)
	{
		s = &_defaultShader;
	}

	if (_boundShader)
	{
		s->Apply(_boundShader);
	}
	else
	{
		s->ForceApply();
	}
	_boundShader = s;

	if (s->IsAlphaTest()) {
		programs.SetAlphaCutoff(s->GetAlphaCutoff());
	} else {
		programs.SetAlphaCutoff(0);
	}
}

void GLShadersManager::ApplyTextureParams() const {
	_boundShader->ApplyTextureParams();
}

