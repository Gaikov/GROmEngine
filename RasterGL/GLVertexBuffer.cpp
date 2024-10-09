/*
 * GLVertexBuffer.cpp
 *
 *  Created on: 12.09.2011
 *      Author: roman
 */

#include "GLVertexBuffer.h"
#include "GLCommon.h"

GLVertexBuffer::GLVertexBuffer(GLTexturesCache *cache, uint numVertices, uint numIndexes, bool useColor) :
        _texturesCache(cache),
		m_numVertices(numVertices),
		m_numIndexes(numIndexes),
		m_useColor(useColor),
		m_maxDrawVertices(0),
		m_maxDrawIndexes(0),
		_primitiveMode(PM_TRIANGLES)
{
	m_verts = new vbVertex_t[m_numVertices];
	m_indexes = new unsigned short[m_numIndexes];
	m_maxDrawIndexes = m_numIndexes;
	m_maxDrawVertices = m_numVertices;
}

GLVertexBuffer::~GLVertexBuffer()
{
	delete[] m_verts;
	delete[] m_indexes;
}

void GLVertexBuffer::Draw()
{
    glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(vbVertex_t), m_verts);

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, sizeof(vbVertex_t), &m_verts->n);

    if (_texturesCache->HasBoundTexture()) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(vbVertex_t), &m_verts->tu);
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

	if (m_useColor)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vbVertex_t), &m_verts->c);
	} else
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	GLenum modes[] = {GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES};
	auto mode = modes[_primitiveMode];

	glDrawElements(mode, m_maxDrawIndexes, GL_UNSIGNED_SHORT, m_indexes);
}

void GLVertexBuffer::SetTex(int vertexIndex, float tu, float tv)
{
	vbVertex_t &v = m_verts[vertexIndex];
	v.tu = tu;
	v.tv = tv;
}

void GLVertexBuffer::SetValidVertices(uint count)
{
	m_maxDrawVertices = count;
}

uint GLVertexBuffer::GetValidVertices()
{
	return m_maxDrawVertices;
}

void GLVertexBuffer::SetValidIndices(uint count)
{
	m_maxDrawIndexes = count;
}

uint GLVertexBuffer::GetValidIndices()
{
	return m_maxDrawIndexes;
}

void GLVertexBuffer::SetPrimitiveMode(primitiveMode_t mode)
{
	_primitiveMode = mode;
}

uint GLVertexBuffer::GetVerticesCount()
{
	return m_numVertices;
}

uint GLVertexBuffer::GetIndicesCount()
{
	return m_numIndexes;
}

vbVertex_t *GLVertexBuffer::GetReadVertices()
{
	return m_verts;
}

vbVertex_t *GLVertexBuffer::GetWriteVertices()
{
	return m_verts;
}

word *GLVertexBuffer::GetReadIndices()
{
	return m_indexes;
}

word *GLVertexBuffer::GetWriteIndices()
{
	return m_indexes;
}

void GLVertexBuffer::SetPos(int vertexIndex, float x, float y, float z)
{
	vbVertex_t &v = m_verts[vertexIndex];
	v.v.x = x;
	v.v.y = y;
	v.v.z = z;
}

void GLVertexBuffer::SetIndex(int index, unsigned short vertexIndex)
{
	m_indexes[index] = vertexIndex;
}
