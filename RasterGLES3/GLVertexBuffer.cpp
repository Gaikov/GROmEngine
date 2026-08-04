/*
 * GLVertexBuffer.cpp
 *
 *  Created on: 12.09.2011
 *      Author: roman
 */

#include "GLVertexBuffer.h"
#include "GLCommon.h"
#include "GLUtils.h"
#include "Core/RenderStats.h"

// Attribute locations for GLES3 shaders:
// 0 - position (vec3), 1 - normal (vec3), 2 - color (rgba8 normalized), 3 - texcoord (vec2)
static constexpr GLuint ATTR_POS = 0;
static constexpr GLuint ATTR_NORM = 1;
static constexpr GLuint ATTR_COLOR = 2;
static constexpr GLuint ATTR_TEX = 3;

GLVertexBuffer::GLVertexBuffer(GLTexturesCache *cache,
    const uint numVertices,
    const uint numIndexes,
    const bool dynamic,
    const bool useColor)
    : _texturesCache(cache),
      m_numVertices(numVertices),
      m_numIndexes(numIndexes),
      m_useColor(useColor),
      m_dynamic(dynamic),
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

    if (!m_dynamic) {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        const auto vertexCapacity = sizeof(vbVertex_t) * m_numVertices;
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertexCapacity),
                     nullptr,
                     GL_STATIC_DRAW);
        nsRenderStats::AddBufferStorageAllocation(vertexCapacity);
    }

    // EBO
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    const auto indexCapacity = sizeof(unsigned short) * m_numIndexes;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indexCapacity),
                 nullptr,
                 GL_STATIC_DRAW);
    nsRenderStats::AddBufferStorageAllocation(indexCapacity);

    if (!m_dynamic) SetupVertexAttributes();

    // Unbind VAO to avoid accidental state changes
    glBindVertexArray(0);
    m_verticesDirty = true;
    m_indicesDirty = true;
}

void GLVertexBuffer::SetupVertexAttributes() const {
    glEnableVertexAttribArray(ATTR_POS);
    glVertexAttribPointer(ATTR_POS,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, v)));

    glEnableVertexAttribArray(ATTR_NORM);
    glVertexAttribPointer(ATTR_NORM,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, n)));

    glEnableVertexAttribArray(ATTR_COLOR);
    glVertexAttribPointer(ATTR_COLOR,
                          4,
                          GL_UNSIGNED_BYTE,
                          GL_TRUE,
                          sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, c)));

    glEnableVertexAttribArray(ATTR_TEX);
    glVertexAttribPointer(ATTR_TEX,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(vbVertex_t),
                          reinterpret_cast<const void *>(offsetof(vbVertex_t, tu)));
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
    for (auto &frameBuffers : m_dynamicBuffers) {
        for (auto &buffer : frameBuffers) {
            if (buffer.id) glDeleteBuffers(1, &buffer.id);
            buffer.id = 0;
            buffer.capacity = 0;
        }
    }
    m_currentDynamicBuffer = 0;
    m_dynamicFrame = 0;
    m_dynamicBufferIndex = 0;
    if (_vao) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
}

std::size_t GLVertexBuffer::NextDynamicCapacity(const std::size_t required) {
    std::size_t capacity = 256;
    while (capacity < required) capacity *= 2;
    return capacity;
}

GLuint GLVertexBuffer::UploadDynamicVertices(const std::uint64_t frameSerial) {
    if (m_dynamicFrame != frameSerial) {
        m_dynamicFrame = frameSerial;
        m_dynamicBufferIndex = 0;
        m_currentDynamicBuffer = 0;
    }

    if (!m_verticesDirty && m_currentDynamicBuffer) return m_currentDynamicBuffer;

    const auto frameSlot = static_cast<std::size_t>(frameSerial % FRAME_SLOT_COUNT);
    auto &buffers = m_dynamicBuffers[frameSlot];
    if (m_dynamicBufferIndex >= buffers.size()) buffers.emplace_back();
    auto &buffer = buffers[m_dynamicBufferIndex++];

    const std::size_t vertexSize = sizeof(vbVertex_t) * m_maxDrawVertices;
    if (!buffer.id) glGenBuffers(1, &buffer.id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
    if (buffer.capacity < vertexSize) {
        buffer.capacity = NextDynamicCapacity(vertexSize);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(buffer.capacity),
                     nullptr,
                     GL_STREAM_DRAW);
        nsRenderStats::AddBufferStorageAllocation(buffer.capacity);
    }
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    static_cast<GLsizeiptr>(vertexSize),
                    m_verts);
    nsRenderStats::AddVertexUpload(vertexSize);
    nsRenderStats::SetDynamicBufferHighWater(buffer.capacity);

    m_verticesDirty = false;
    m_currentDynamicBuffer = buffer.id;
    return buffer.id;
}

void GLVertexBuffer::Draw(const std::uint64_t frameSerial) {
    if (!m_maxDrawVertices || !m_maxDrawIndexes) return;
    if (!_vao || !_ebo || (!m_dynamic && !_vbo)) {
        InitGLObjects();
    }

    glBindVertexArray(_vao);

    if (m_dynamic) {
        glBindBuffer(GL_ARRAY_BUFFER, UploadDynamicVertices(frameSerial));
        SetupVertexAttributes();
    } else if (m_verticesDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        const auto vertSize = static_cast<GLsizeiptr>(sizeof(vbVertex_t) * m_maxDrawVertices);
        if (vertSize > 0) glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, m_verts);
        nsRenderStats::AddVertexUpload(static_cast<std::size_t>(vertSize));
        m_verticesDirty = false;
    }

    if (m_indicesDirty) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        const auto idxSize = static_cast<GLsizeiptr>(sizeof(unsigned short) * m_numIndexes);
        if (idxSize > 0) glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idxSize, m_indexes);
        nsRenderStats::AddIndexUpload(static_cast<std::size_t>(idxSize));
        m_indicesDirty = false;
    }

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
    nsRenderStats::AddDrawCall();
    GL_CHECK_HOT_R("glDrawElements",);

    glBindVertexArray(0);
}


void GLVertexBuffer::SetTex(int vertexIndex, float tu, float tv) {
    vbVertex_t &v = m_verts[vertexIndex];
    v.tu = tu;
    v.tv = tv;
    m_verticesDirty = true;
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
    m_verticesDirty = true;
    return m_verts;
}

word *GLVertexBuffer::GetReadIndices() {
    return m_indexes;
}

word *GLVertexBuffer::GetWriteIndices() {
    m_indicesDirty = true;
    return m_indexes;
}

void GLVertexBuffer::SetPos(int vertexIndex, float x, float y, float z) {
    vbVertex_t &v = m_verts[vertexIndex];
    v.v.x = x;
    v.v.y = y;
    v.v.z = z;
    m_verticesDirty = true;
}

void GLVertexBuffer::SetIndex(int index, unsigned short vertexIndex) {
    m_indexes[index] = vertexIndex;
    m_indicesDirty = true;
}

void GLVertexBuffer::UseColor(const nsColor &color) {
    _color = color;
}

void GLVertexBuffer::Invalidate() {
    ReleaseGLObjects();
    m_verticesDirty = true;
    m_indicesDirty = true;
}
