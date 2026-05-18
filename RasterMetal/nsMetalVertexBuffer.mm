//
// Created by OpenCode
//

#include "nsMetalVertexBuffer.h"
#include "nsMetalTexturesCache.h"

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
    _vertexBuffer = [_device newBufferWithLength:sizeof(vbVertex_t) * _numVertices
                                        options:MTLResourceStorageModeShared];
    _indexBuffer = [_device newBufferWithLength:sizeof(unsigned short) * _numIndexes
                                       options:MTLResourceStorageModeShared];
}

void nsMetalVertexBuffer::ReleaseBuffers() {
    _vertexBuffer = nil;
    _indexBuffer = nil;
}

void nsMetalVertexBuffer::Draw(id<MTLRenderCommandEncoder> encoder) {
    if (!encoder || _maxDrawVertices == 0 || _maxDrawIndexes == 0) return;

    const auto vertexBytes = sizeof(vbVertex_t) * _maxDrawVertices;
    const auto indexBytes = sizeof(unsigned short) * _maxDrawIndexes;
    id<MTLBuffer> vertexBuffer = [_device newBufferWithBytes:_verts
                                                       length:vertexBytes
                                                      options:MTLResourceStorageModeShared];
    id<MTLBuffer> indexBuffer = [_device newBufferWithBytes:_indexes
                                                      length:indexBytes
                                                     options:MTLResourceStorageModeShared];
    if (!vertexBuffer || !indexBuffer) return;

    [encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];

    MTLPrimitiveType mode;
    switch (_primitiveMode) {
        case PM_TRISTRIPS: mode = MTLPrimitiveTypeTriangleStrip; break;
        case PM_LINES:     mode = MTLPrimitiveTypeLine;          break;
        default:           mode = MTLPrimitiveTypeTriangle;      break;
    }

    [encoder drawIndexedPrimitives:mode
                       indexCount:_maxDrawIndexes
                        indexType:MTLIndexTypeUInt16
                      indexBuffer:indexBuffer
                 indexBufferOffset:0];
}

void nsMetalVertexBuffer::SetPos(int vertexIndex, float x, float y, float z) {
    if (vertexIndex < 0 || (uint)vertexIndex >= _numVertices) return;
    auto &v = _verts[vertexIndex];
    v.v.x = x; v.v.y = y; v.v.z = z;
}

void nsMetalVertexBuffer::SetTex(int vertexIndex, float tu, float tv) {
    if (vertexIndex < 0 || (uint)vertexIndex >= _numVertices) return;
    auto &v = _verts[vertexIndex];
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
    _maxDrawVertices = count <= _numVertices ? count : _numVertices;
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
    return _verts;
}

word *nsMetalVertexBuffer::GetReadIndices() {
    return _indexes;
}

word *nsMetalVertexBuffer::GetWriteIndices() {
    return _indexes;
}
