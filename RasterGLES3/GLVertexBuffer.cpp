/*
 * GLVertexBuffer.cpp
 *
 *  Created on: 12.09.2011
 *      Author: roman
 */

#include "GLVertexBuffer.h"
#include "GLCommon.h"
#include "GLUtils.h"

// Attribute locations for GLES3 shaders:
// 0 - position (vec3), 1 - normal (vec3), 2 - color (rgba8 normalized), 3 - texcoord (vec2)
static constexpr GLuint ATTR_POS = 0;
static constexpr GLuint ATTR_NORM = 1;
static constexpr GLuint ATTR_COLOR = 2;
static constexpr GLuint ATTR_TEX = 3;

GLVertexBuffer::GLVertexBuffer(GLTexturesCache *cache,
    const uint numVertices,
    const uint numIndexes,
    const bool useColor)
    : _texturesCache(cache),
      m_numVertices(numVertices),
      m_numIndexes(numIndexes),
      m_useColor(useColor),
      _primitiveMode(PM_TRIANGLES),
      m_maxDrawVertices(0),
      m_maxDrawIndexes(0) {
    m_verts = new vbVertex_t[m_numVertices];
    m_indexes = new unsigned short[m_numIndexes];
    m_maxDrawIndexes = m_numIndexes;
    m_maxDrawVertices = m_numVertices;

    InitGLObjects();
}

GLVertexBuffer::~GLVertexBuffer() {
    ReleaseGLObjects();

    delete[] m_verts;
    delete[] m_indexes;
}

void GLVertexBuffer::InitGLObjects() {
    Log::Info("...initializing GL vertex buffer");

    // VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // VBO
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vbVertex_t) * m_numVertices, nullptr, GL_DYNAMIC_DRAW);

    // EBO
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_numIndexes, nullptr, GL_DYNAMIC_DRAW);

    // Setup vertex attributes layout (interleaved vbVertex_t)
    // position: vec3 float at offset of nsVec3 v
    glEnableVertexAttribArray(ATTR_POS);
    glVertexAttribPointer(ATTR_POS, 3, GL_FLOAT, GL_FALSE, sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, v)));

    // normal: vec3 float at offset nsVec3 n
    glEnableVertexAttribArray(ATTR_NORM);
    glVertexAttribPointer(ATTR_NORM, 3, GL_FLOAT, GL_FALSE, sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, n)));

    // color: rgba8 normalized at offset dword c
    glEnableVertexAttribArray(ATTR_COLOR);
    glVertexAttribPointer(ATTR_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, c)));

    // texcoord: vec2 float at offset float tu
    glEnableVertexAttribArray(ATTR_TEX);
    glVertexAttribPointer(ATTR_TEX, 2, GL_FLOAT, GL_FALSE, sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, tu)));

    // Unbind VAO to avoid accidental state changes
    glBindVertexArray(0);
}

void GLVertexBuffer::ReleaseGLObjects() {
    if (_ebo) {
        glDeleteBuffers(1, &_ebo);
        _ebo = 0;
    }
    if (_vbo) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
}

void GLVertexBuffer::Draw() {
    if (!_vao) {
        InitGLObjects();
    }

    // Upload current CPU-side data to GPU buffers
    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    const auto vertSize = static_cast<GLsizeiptr>(sizeof(vbVertex_t) * m_maxDrawVertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, m_verts);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    const auto idxSize = static_cast<GLsizeiptr>(sizeof(unsigned short) * m_maxDrawIndexes);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idxSize, m_indexes);

    // Enable/disable attributes depending on state
    if (_texturesCache->HasBoundTexture()) {
        glEnableVertexAttribArray(ATTR_TEX);
    } else {
        glDisableVertexAttribArray(ATTR_TEX);
    }

    if (m_useColor) {
        glEnableVertexAttribArray(ATTR_COLOR);
    } else {
        glDisableVertexAttribArray(ATTR_COLOR);
        glVertexAttrib4fv(ATTR_COLOR, _color);
    }

    constexpr GLenum modes[] = {GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES};
    const auto mode = modes[_primitiveMode];

    glDrawElements(mode, static_cast<GLsizei>(m_maxDrawIndexes), GL_UNSIGNED_SHORT, nullptr);
    GL_CHECK_R("glDrawElements",);

    glBindVertexArray(0);
}


void GLVertexBuffer::SetTex(int vertexIndex, float tu, float tv) {
    vbVertex_t &v = m_verts[vertexIndex];
    v.tu = tu;
    v.tv = tv;
}

void GLVertexBuffer::SetValidVertices(uint count) {
    m_maxDrawVertices = count;
}

uint GLVertexBuffer::GetValidVertices() {
    return m_maxDrawVertices;
}

void GLVertexBuffer::SetValidIndices(uint count) {
    m_maxDrawIndexes = count;
}

uint GLVertexBuffer::GetValidIndices() {
    return m_maxDrawIndexes;
}

void GLVertexBuffer::SetPrimitiveMode(primitiveMode_t mode) {
    _primitiveMode = mode;
}

uint GLVertexBuffer::GetVerticesCount() {
    return m_numVertices;
}

uint GLVertexBuffer::GetIndicesCount() {
    return m_numIndexes;
}

vbVertex_t *GLVertexBuffer::GetReadVertices() {
    return m_verts;
}

vbVertex_t *GLVertexBuffer::GetWriteVertices() {
    return m_verts;
}

word *GLVertexBuffer::GetReadIndices() {
    return m_indexes;
}

word *GLVertexBuffer::GetWriteIndices() {
    return m_indexes;
}

void GLVertexBuffer::SetPos(int vertexIndex, float x, float y, float z) {
    vbVertex_t &v = m_verts[vertexIndex];
    v.v.x = x;
    v.v.y = y;
    v.v.z = z;
}

void GLVertexBuffer::SetIndex(int index, unsigned short vertexIndex) {
    m_indexes[index] = vertexIndex;
}

void GLVertexBuffer::UseColor(const nsColor &color) {
    _color = color;
}

void GLVertexBuffer::Invalidate() {
    ReleaseGLObjects();
}
