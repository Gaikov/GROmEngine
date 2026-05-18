//
// Created by OpenCode
//
#pragma once

#include "Engine/RenDevice.h"
#include "nsMetalConfig.h"
#include "nsLib/color.h"
#include <vector>

#import <Metal/Metal.h>

class nsMetalTexturesCache;

class nsMetalVertexBuffer final : public IVertexBuffer {
public:
    nsMetalVertexBuffer(id<MTLDevice> device,
                         nsMetalTexturesCache *cache,
                         uint numVertices,
                         uint numIndexes,
                         bool useColor);
    ~nsMetalVertexBuffer() override;

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

    void Draw(id<MTLRenderCommandEncoder> encoder, uint frameIndex);
    bool UsesColor() const { return _useColor; }
    void SetPos(int vertexIndex, float x, float y, float z);
    void SetTex(int vertexIndex, float tu, float tv);
    void SetIndex(int index, unsigned short vertexIndex);
    void UseColor(const nsColor &color);
    void Invalidate();

private:
    id<MTLDevice>           _device          = nil;
    nsMetalTexturesCache   *_texturesCache   = nullptr;

    vbVertex_t             *_verts           = nullptr;
    unsigned short          *_indexes         = nullptr;
    uint                    _numVertices      = 0;
    uint                    _numIndexes       = 0;
    bool                    _useColor         = false;
    primitiveMode_t         _primitiveMode    = PM_TRIANGLES;
    nsColor                 _color;

    std::vector<id<MTLBuffer>> _vertexBuffers[kMetalInFlightFrameSlots];
    std::vector<id<MTLBuffer>> _indexBuffers[kMetalInFlightFrameSlots];

    uint                    _maxDrawVertices  = 0;
    uint                    _maxDrawIndexes   = 0;
    uint                    _lastFrameIndex   = ~0u;
    uint                    _drawSlot         = 0;

    void InitBuffers();
    void ReleaseBuffers();
    bool EnsureBuffers(uint frameSlot, uint drawSlot);
};
