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
	Apply(&_defaultShader);
	return true;
}

void GLShadersManager::Apply(GLShader *shader)
{
	if (!shader)
	{
		shader = &_defaultShader;
	}

	if (_boundShader)
	{
		shader->Apply(_boundShader);
	}
	else
	{
		shader->ForceApply();
	}
	_boundShader = shader;
}

void GLShadersManager::ApplyTextureParams()
{
	_boundShader->ApplyTextureParams();
}

