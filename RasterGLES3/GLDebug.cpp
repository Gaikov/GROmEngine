//
// Created by Raman Haikou on 2019-07-22.
//

#include "GLDebug.h"
#include "GLTexture.h"
#include "GLVertexBuffer.h"


void GLDebug::DrawQuad(float x, float y, float width, float height)
{
	_quadBuffer->SetPos(0, x, y, 0);
	_quadBuffer->SetPos(1, x, y + height, 0);
	_quadBuffer->SetPos(2, x + width, y + height, 0);
	_quadBuffer->SetPos(3, x + width, y, 0);
	_quadBuffer->Draw();
}

void GLDebug::DrawSprite(GLTexture *t, float x, float y)
{
	_texturesCache->BindTexture(t);
	DrawQuad(x, y, t->GetWidth(), t->GetHeight());
}

/*
 * 0    3
 *
 * 1    2
 *
 * triangle 1: 0 1 2
 * triangle 2: 0 2 3
 * */

bool GLDebug::Init(GLTexturesCache *cache)
{
    _texturesCache = cache;

	_quadBuffer = new GLVertexBuffer(_texturesCache, 4, 6, false);
	_quadBuffer->SetValidVertices(4);
	_quadBuffer->SetValidIndices(6);
	_quadBuffer->SetTex(0, 0, 0);
	_quadBuffer->SetTex(1, 0, 1);
	_quadBuffer->SetTex(2, 1, 1);
	_quadBuffer->SetTex(3, 1, 0);

	_quadBuffer->SetIndex(0, 0);
	_quadBuffer->SetIndex(1, 1);
	_quadBuffer->SetIndex(2, 2);

	_quadBuffer->SetIndex(3, 0);
	_quadBuffer->SetIndex(4, 2);
	_quadBuffer->SetIndex(5, 3);

	_lineBuffer = new GLVertexBuffer(_texturesCache, 2, 2, true);
	_lineBuffer->SetPrimitiveMode(PM_LINES);
	_lineBuffer->SetValidIndices(2);
	_lineBuffer->SetValidIndices(2);

	_lineBuffer->SetIndex(0, 0);
	_lineBuffer->SetIndex(1, 1);

	return true;
}

void GLDebug::Release()
{
	delete _lineBuffer;
	delete _quadBuffer;
}

void GLDebug::DrawLine(float x1, float y1, float x2, float y2, dword color)
{
	_lineBuffer->SetPos(0, x1, y1, 0);
	_lineBuffer->SetPos(1, x2, y2, 0);

	auto v = _lineBuffer->GetWriteVertices();
	v[0].c = color;
	v[1].c = color;

	_lineBuffer->Draw();
}

