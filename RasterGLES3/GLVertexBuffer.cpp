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
      _numVertices(numVertices),
      _numIndexes(numIndexes),
      _useColor(useColor),
      _dynamic(dynamic),
      _primitiveMode(PM_TRIANGLES),
      _maxDrawVertices(0),
      _maxDrawIndexes(0) {
    _verts = new vbVertex_t[_numVertices];
    _indexes = new unsigned short[_numIndexes];
    _maxDrawIndexes = _numIndexes;
    _maxDrawVertices = _numVertices;

    InitGLObjects();
}

GLVertexBuffer::~GLVertexBuffer() {
    ReleaseGLObjects();

    delete[] _verts;
    delete[] _indexes;
}

void GLVertexBuffer::InitGLObjects() {
    Log::Info("...initializing GL vertex buffer");

    // VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    if (!_dynamic) {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        const auto vertexCapacity = sizeof(vbVertex_t) * _numVertices;
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertexCapacity),
                     nullptr,
                     GL_STATIC_DRAW);
        nsRenderStats::AddBufferStorageAllocation(vertexCapacity);
    }

    // EBO
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    const auto indexCapacity = sizeof(unsigned short) * _numIndexes;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indexCapacity),
                 nullptr,
                 GL_STATIC_DRAW);
    nsRenderStats::AddBufferStorageAllocation(indexCapacity);

    if (!_dynamic) SetupVertexAttributes();

    // Unbind VAO to avoid accidental state changes
    glBindVertexArray(0);
    _verticesDirty = true;
    _indicesDirty = true;
    _uploadedIndices = 0;
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
    for (auto &frameBuffers : _dynamicBuffers) {
        for (auto &buffer : frameBuffers) {
            if (buffer.id) glDeleteBuffers(1, &buffer.id);
            buffer.id = 0;
            buffer.capacity = 0;
        }
    }
    _currentDynamicBuffer = 0;
    _dynamicFrame = 0;
    _dynamicBufferIndex = 0;
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
    if (_dynamicFrame != frameSerial) {
        _dynamicFrame = frameSerial;
        _dynamicBufferIndex = 0;
        _currentDynamicBuffer = 0;
    }

    if (!_verticesDirty && _currentDynamicBuffer) return _currentDynamicBuffer;

    const auto frameSlot = static_cast<std::size_t>(frameSerial % FRAME_SLOT_COUNT);
    auto &buffers = _dynamicBuffers[frameSlot];
    if (_dynamicBufferIndex >= buffers.size()) buffers.emplace_back();
    auto &buffer = buffers[_dynamicBufferIndex++];

    const std::size_t vertexSize = sizeof(vbVertex_t) * _maxDrawVertices;
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
                    _verts);
    nsRenderStats::AddVertexUpload(vertexSize);
    nsRenderStats::SetDynamicBufferHighWater(buffer.capacity);

    _verticesDirty = false;
    _currentDynamicBuffer = buffer.id;
    return buffer.id;
}

void GLVertexBuffer::Draw(const std::uint64_t frameSerial) {
    if (!_maxDrawVertices || !_maxDrawIndexes) return;
    if (!_vao || !_ebo || (!_dynamic && !_vbo)) {
        InitGLObjects();
    }

    glBindVertexArray(_vao);

    if (_dynamic) {
        glBindBuffer(GL_ARRAY_BUFFER, UploadDynamicVertices(frameSerial));
        SetupVertexAttributes();
    } else if (_verticesDirty) {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        const auto vertSize = static_cast<GLsizeiptr>(sizeof(vbVertex_t) * _maxDrawVertices);
        if (vertSize > 0) glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, _verts);
        nsRenderStats::AddVertexUpload(static_cast<std::size_t>(vertSize));
        _verticesDirty = false;
    }

    // Active range changes reuse the uploaded prefix until the CPU array changes.
    if (_indicesDirty || _maxDrawIndexes > _uploadedIndices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
        const auto idxSize = static_cast<GLsizeiptr>(sizeof(unsigned short) * _maxDrawIndexes);
        if (idxSize > 0) glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idxSize, _indexes);
        nsRenderStats::AddIndexUpload(static_cast<std::size_t>(idxSize));
        _indicesDirty = false;
        _uploadedIndices = _maxDrawIndexes;
    }

    // Enable/disable attributes depending on state
    if (_texturesCache->HasBoundTexture()) {
        glEnableVertexAttribArray(ATTR_TEX);
    } else {
        glDisableVertexAttribArray(ATTR_TEX);
    }

    if (_useColor) {
        glEnableVertexAttribArray(ATTR_COLOR);
    } else {
        glDisableVertexAttribArray(ATTR_COLOR);
        glVertexAttrib4fv(ATTR_COLOR, _color);
    }

    constexpr GLenum modes[] = {GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES};
    const auto mode = modes[_primitiveMode];

    glDrawElements(mode, static_cast<GLsizei>(_maxDrawIndexes), GL_UNSIGNED_SHORT, nullptr);
    nsRenderStats::AddDrawCall();
    GL_CHECK_HOT_R("glDrawElements",);

    glBindVertexArray(0);
}


void GLVertexBuffer::SetTex(int vertexIndex, float tu, float tv) {
    vbVertex_t &v = _verts[vertexIndex];
    v.tu = tu;
    v.tv = tv;
    _verticesDirty = true;
}

void GLVertexBuffer::SetValidVertices(uint count) {
    _maxDrawVertices = count;
}

uint GLVertexBuffer::GetValidVertices() {
    return _maxDrawVertices;
}

void GLVertexBuffer::SetValidIndices(uint count) {
    _maxDrawIndexes = std::min(count, _numIndexes);
}

uint GLVertexBuffer::GetValidIndices() {
    return _maxDrawIndexes;
}

void GLVertexBuffer::SetPrimitiveMode(primitiveMode_t mode) {
    _primitiveMode = mode;
}

uint GLVertexBuffer::GetVerticesCount() {
    return _numVertices;
}

uint GLVertexBuffer::GetIndicesCount() {
    return _numIndexes;
}

vbVertex_t *GLVertexBuffer::GetReadVertices() {
    return _verts;
}

vbVertex_t *GLVertexBuffer::GetWriteVertices() {
    _verticesDirty = true;
    return _verts;
}

word *GLVertexBuffer::GetReadIndices() {
    return _indexes;
}

word *GLVertexBuffer::GetWriteIndices() {
    _indicesDirty = true;
    return _indexes;
}

void GLVertexBuffer::SetPos(int vertexIndex, float x, float y, float z) {
    vbVertex_t &v = _verts[vertexIndex];
    v.v.x = x;
    v.v.y = y;
    v.v.z = z;
    _verticesDirty = true;
}

void GLVertexBuffer::SetIndex(int index, unsigned short vertexIndex) {
    _indexes[index] = vertexIndex;
    _indicesDirty = true;
}

void GLVertexBuffer::UseColor(const nsColor &color) {
    _color = color;
}

void GLVertexBuffer::Invalidate() {
    ReleaseGLObjects();
    _verticesDirty = true;
    _indicesDirty = true;
}
