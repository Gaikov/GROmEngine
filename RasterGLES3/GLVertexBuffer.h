/*
 * GLVertexBuffer.h
 *
 *  Created on: 12.09.2025
 *      Author: roman
 */

#pragma once

#include "GLCommon.h"
#include "GLRenderDevice.h"
#include "GLTexturesCache.h"

#include <cstddef>
#include <cstdint>
#include <vector>

class GLVertexBuffer final : public IVertexBuffer
{
public:
	GLVertexBuffer(GLTexturesCache *cache, uint numVertices, uint numIndexes, bool dynamic, bool useColor);
	~GLVertexBuffer() override;

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

	void Draw(std::uint64_t frameSerial);

	void SetPos(int vertexIndex, float x, float y, float z);
	void SetTex(int vertexIndex, float tu, float tv);
	void SetIndex(int index, unsigned short vertexIndex);
	void UseColor(const nsColor &color);
	void Invalidate();

private:
    GLTexturesCache *_texturesCache;

	vbVertex_t *m_verts;
	unsigned short *m_indexes;

	uint m_numVertices;
	uint m_numIndexes;
	bool m_useColor;
	bool m_dynamic;
	bool m_verticesDirty = true;
	bool m_indicesDirty = true;

	static constexpr std::size_t FRAME_SLOT_COUNT = 3;
	struct DynamicBuffer {
		GLuint id = 0;
		std::size_t capacity = 0;
	};
	std::vector<DynamicBuffer> m_dynamicBuffers[FRAME_SLOT_COUNT];
	std::uint64_t m_dynamicFrame = 0;
	std::size_t m_dynamicBufferIndex = 0;
	GLuint m_currentDynamicBuffer = 0;

	primitiveMode_t _primitiveMode;

	unsigned int _vao = 0;
	unsigned int _vbo = 0;
	unsigned int _ebo = 0;
	nsColor _color;

	void InitGLObjects();
	void ReleaseGLObjects();
	void SetupVertexAttributes() const;
	GLuint UploadDynamicVertices(std::uint64_t frameSerial);
	static std::size_t NextDynamicCapacity(std::size_t required);

	uint m_maxDrawVertices;
	uint m_maxDrawIndexes;
};
