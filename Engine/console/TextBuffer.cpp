//
// Created by Roman on 9/24/2025.
//

#include "TextBuffer.h"
#include "nsLib/log.h"

nsTextBuffer::nsTextBuffer(const int maxChunkSize): _maxChunkSize(maxChunkSize) {
    _currentBuffer = nsStringBuffer::Create(_maxChunkSize);
    _buffers.push_back(_currentBuffer);
    Log::Info("...allocated console buffer, chunk size: %i", _maxChunkSize);
}

nsTextBuffer::~nsTextBuffer() {
    for (const auto buffer : _buffers) {
        delete buffer;
    }
    Log::Info("...released console buffer, chunks: %i", _buffers.size());
}

char * nsTextBuffer::Allocate(const int size) {
    assert(size <= _maxChunkSize);

    auto res = _currentBuffer->Allocate(size);
    if (!res) {
        _currentBuffer = nsStringBuffer::Create(_maxChunkSize);
        _buffers.push_back(_currentBuffer);
        res = _currentBuffer->Allocate(size);
    }
    return res;
}
