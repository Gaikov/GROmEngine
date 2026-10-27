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

class GLVertexBuffer final : public IVertexBuffer {
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

    vbVertex_t *_verts;
    unsigned short *_indexes;

    uint _numVertices;
    uint _numIndexes;
    bool _useColor;
    bool _dynamic;
    bool _verticesDirty = true;
    bool _indicesDirty = true;
    uint _uploadedIndices = 0;

    static constexpr std::size_t FRAME_SLOT_COUNT = 3;
    struct DynamicBuffer {
        GLuint id = 0;
        std::size_t capacity = 0;
    };
    std::vector<DynamicBuffer> _dynamicBuffers[FRAME_SLOT_COUNT];
    std::uint64_t _dynamicFrame = 0;
    std::size_t _dynamicBufferIndex = 0;
    GLuint _currentDynamicBuffer = 0;

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

    uint _maxDrawVertices;
    uint _maxDrawIndexes;
};
