//
// Created by OpenCode
//

#include "nsMetalVertexBuffer.h"
#include "nsMetalTexturesCache.h"
#include "Core/Memory.h"
#include "Core/RenderStats.h"

namespace {
// Keep small submissions small and grow each reusable slot geometrically.
size_t StreamingCapacity(size_t required, size_t maximum) {
    auto capacity = size_t{256};
    while (capacity < required) capacity *= 2;
    return std::min(capacity, maximum);
}
}

nsMetalVertexBuffer::nsMetalVertexBuffer(id<MTLDevice> device,
                                           nsMetalTexturesCache *cache,
                                           uint numVertices,
                                           uint numIndexes,
                                           bool useColor)
    : _device(device)
    , _texturesCache(cache)
    , _numVertices(numVertices)
    , _numIndexes(numIndexes)
    , _useColor(useColor)
    , _maxDrawVertices(numVertices)
    , _maxDrawIndexes(numIndexes) {
    _verts = new vbVertex_t[_numVertices];
    _indexes = new unsigned short[_numIndexes];
    memset(_verts, 0, sizeof(vbVertex_t) * _numVertices);
    memset(_indexes, 0, sizeof(unsigned short) * _numIndexes);
    InitBuffers();
}

nsMetalVertexBuffer::~nsMetalVertexBuffer() {
    ReleaseBuffers();
    delete[] _verts;
    delete[] _indexes;
}

void nsMetalVertexBuffer::InitBuffers() {
    ReleaseBuffers();
    _lastFrameIndex = ~0u;
    _drawSlot = 0;
}

void nsMetalVertexBuffer::ReleaseBuffers() {
    for (uint i = 0; i < kMetalInFlightFrameSlots; ++i) {
        _vertexBuffers[i].clear();
        _vertexVersions[i].clear();
        _indexBuffers[i].clear();
    }
}

bool nsMetalVertexBuffer::EnsureBuffers(uint frameSlot, uint drawSlot) {
    auto &vertexBuffers = _vertexBuffers[frameSlot];
    auto &indexBuffers = _indexBuffers[frameSlot];
    const auto vertexBytes = sizeof(vbVertex_t) * _maxDrawVertices;
    const auto indexBytes = sizeof(unsigned short) * _maxDrawIndexes;
    if (drawSlot < vertexBuffers.size() && vertexBuffers[drawSlot].length >= vertexBytes
        && indexBuffers[drawSlot].length >= indexBytes) {
        return true;
    }

    nsMemoryLoopAllocScope metalAllocScope;
    while (vertexBuffers.size() <= drawSlot) {
        vertexBuffers.push_back(nil);
        indexBuffers.push_back(nil);
        _vertexVersions[frameSlot].push_back(0);
    }
    auto &vertexBuffer = vertexBuffers[drawSlot];
    if (vertexBuffer.length < vertexBytes) {
        const auto capacity = StreamingCapacity(vertexBytes, sizeof(vbVertex_t) * _numVertices);
        auto replacement = [_device newBufferWithLength:capacity options:MTLResourceStorageModeShared];
        if (!replacement) return false;
        vertexBuffer = replacement;
        _vertexVersions[frameSlot][drawSlot] = 0;
        nsRenderStats::AddBufferStorageAllocation(capacity);
    }
    auto &indexBuffer = indexBuffers[drawSlot];
    if (indexBuffer.length < indexBytes) {
        const auto capacity = StreamingCapacity(indexBytes, sizeof(unsigned short) * _numIndexes);
        auto replacement = [_device newBufferWithLength:capacity options:MTLResourceStorageModeShared];
        if (!replacement) return false;
        indexBuffer = replacement;
        nsRenderStats::AddBufferStorageAllocation(capacity);
    }
    return true;
}

void nsMetalVertexBuffer::Draw(id<MTLRenderCommandEncoder> encoder, uint frameIndex) {
    if (!encoder || _maxDrawVertices == 0 || _maxDrawIndexes == 0) return;

    if (_lastFrameIndex != frameIndex) {
        _lastFrameIndex = frameIndex;
        _drawSlot = 0;
    }

    const auto frameSlot = frameIndex % kMetalInFlightFrameSlots;
    const auto drawSlot = _drawSlot++;
    if (!EnsureBuffers(frameSlot, drawSlot)) return;

    const auto indexBytes = sizeof(unsigned short) * _maxDrawIndexes;
    id<MTLBuffer> vertexBuffer = _vertexBuffers[frameSlot][drawSlot];
    id<MTLBuffer> indexBuffer = _indexBuffers[frameSlot][drawSlot];
    if (!vertexBuffer || !indexBuffer) return;

    if (_vertexVersions[frameSlot][drawSlot] != _vertexVersion) {
        memcpy([vertexBuffer contents], _verts, sizeof(vbVertex_t) * _maxDrawVertices);
        nsRenderStats::AddVertexUpload(sizeof(vbVertex_t) * _maxDrawVertices);
        _vertexVersions[frameSlot][drawSlot] = _vertexVersion;
    }
    memcpy([indexBuffer contents], _indexes, indexBytes);
    nsRenderStats::AddIndexUpload(indexBytes);
    [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

    MTLPrimitiveType mode;
    switch (_primitiveMode) {
        case PM_TRISTRIPS: mode = MTLPrimitiveTypeTriangleStrip; break;
        case PM_LINES:     mode = MTLPrimitiveTypeLine;          break;
        default:           mode = MTLPrimitiveTypeTriangle;      break;
    }

    nsRenderStats::AddDrawCall();
    [encoder drawIndexedPrimitives:mode
                       indexCount:_maxDrawIndexes
                        indexType:MTLIndexTypeUInt16
                      indexBuffer:indexBuffer
                 indexBufferOffset:0];
}

void nsMetalVertexBuffer::SetPos(int vertexIndex, float x, float y, float z) {
    if (vertexIndex < 0 || (uint)vertexIndex >= _numVertices) return;
    auto &v = _verts[vertexIndex];
    ++_vertexVersion;
    v.v.x = x; v.v.y = y; v.v.z = z;
}

void nsMetalVertexBuffer::SetTex(int vertexIndex, float tu, float tv) {
    if (vertexIndex < 0 || (uint)vertexIndex >= _numVertices) return;
    auto &v = _verts[vertexIndex];
    ++_vertexVersion;
    v.tu = tu; v.tv = tv;
}

void nsMetalVertexBuffer::SetIndex(int index, unsigned short vertexIndex) {
    if (index < 0 || (uint)index >= _numIndexes) return;
    _indexes[index] = vertexIndex;
}

void nsMetalVertexBuffer::UseColor(const nsColor &color) {
    _color = color;
}

void nsMetalVertexBuffer::Invalidate() {
    InitBuffers();
}

void nsMetalVertexBuffer::SetValidVertices(uint count) {
    count = std::min(count, _numVertices);
    if (count > _maxDrawVertices) ++_vertexVersion;
    _maxDrawVertices = count;
}

uint nsMetalVertexBuffer::GetValidVertices() {
    return _maxDrawVertices;
}

void nsMetalVertexBuffer::SetValidIndices(uint count) {
    _maxDrawIndexes = count <= _numIndexes ? count : _numIndexes;
}

uint nsMetalVertexBuffer::GetValidIndices() {
    return _maxDrawIndexes;
}

void nsMetalVertexBuffer::SetPrimitiveMode(primitiveMode_t mode) {
    _primitiveMode = mode;
}

uint nsMetalVertexBuffer::GetVerticesCount() {
    return _numVertices;
}

uint nsMetalVertexBuffer::GetIndicesCount() {
    return _numIndexes;
}

vbVertex_t *nsMetalVertexBuffer::GetReadVertices() {
    return _verts;
}

vbVertex_t *nsMetalVertexBuffer::GetWriteVertices() {
    ++_vertexVersion;
    return _verts;
}

word *nsMetalVertexBuffer::GetReadIndices() {
    return _indexes;
}

word *nsMetalVertexBuffer::GetWriteIndices() {
    return _indexes;
}
