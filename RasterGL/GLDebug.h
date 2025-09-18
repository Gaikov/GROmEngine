//
// Created by Raman Haikou on 2019-07-22.
//

#ifndef SDLTEST_GLDEBUG_H
#define SDLTEST_GLDEBUG_H

#include "GLCommon.h"
#include "GLTexturesCache.h"

class GLTexture;
class GLVertexBuffer;

class GLDebug
{
public:
	bool Init(GLTexturesCache *cache);
	void Release();

	void DrawSprite(GLTexture *t, float x, float y);
	void DrawQuad(float x, float y, float width, float height);
	void DrawLine(float x1, float y1, float x2, float y2, dword color);

private:
    GLTexturesCache *_texturesCache = nullptr;
	GLVertexBuffer *_quadBuffer = nullptr;
	GLVertexBuffer *_lineBuffer = nullptr;
};


#endif //SDLTEST_GLDEBUG_H
