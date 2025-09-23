/*
 * GLVertexBuffer.h
 *
 *  Created on: 12.09.2011
 *      Author: roman
 */

#ifndef VERTEXBUFFER_H_
#define VERTEXBUFFER_H_

#include "GLCommon.h"
#include "GLRenderDevice.h"
#include "GLTexturesCache.h"

class GLVertexBuffer : public IVertexBuffer
{
public:
	GLVertexBuffer(GLTexturesCache *cache, uint numVertices, uint numIndexes, bool useColor);
	~GLVertexBuffer();

	void SetValidVertices(uint count) override;
	uint GetValidVertices() override;
	void SetValidIndices(uint count) override;
	uint GetValidIndices() override;
	void SetPrimitiveMode(primitiveMode_t mode) override;
	uint GetVerticesCount() override;
	uint GetIndicesCount() override;
	vbVertex_t *GetReadVertices() override;
	vbVertex_t *GetWriteVertices() override;
	word *GetReadIndices() override;
	word *GetWriteIndices() override;

	void Draw();

	void SetPos(int vertexIndex, float x, float y, float z);
	void SetTex(int vertexIndex, float tu, float tv);
	void SetIndex(int index, unsigned short vertexIndex);
	void UseColor(const nsColor &color);

private:
    GLTexturesCache *_texturesCache;

	vbVertex_t *m_verts;
	unsigned short *m_indexes;

	uint m_numVertices;
	uint m_numIndexes;
	bool m_useColor;

	primitiveMode_t _primitiveMode;

	unsigned int _vao = 0;
	unsigned int _vbo = 0;
	unsigned int _ebo = 0;
	nsColor _color;

	void InitGLObjects();
	void ReleaseGLObjects();

	uint m_maxDrawVertices;
	uint m_maxDrawIndexes;
};

#endif /* VERTEXBUFFER_H_ */
