//
// Created by Roman on 9/24/2025.
//

#include "StringBuffer.h"
#include "Core/Memory.h"

nsStringBuffer::nsStringBuffer(const int maxSize) : _size(maxSize) {
    _buffer = static_cast<char *>(my_malloc(_size));
}

nsStringBuffer::~nsStringBuffer() {
    my_free(_buffer);
    _buffer = nullptr;
    _size = 0;
    _pos = 0;
}

char * nsStringBuffer::Allocate(const int size) {
    const int remaining = _size - _pos;
    if (size > remaining) {
        return nullptr;
    }

    const int start = _pos;
    _pos += size;
    return _buffer + start;
}
