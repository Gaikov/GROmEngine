//
// Created by Roman on 1/16/2026.
//

#include "StringWriter.h"

static char out[1024];

bool nsStringWriter::Printf(const char *fmt, ...) {
    va_list	list;
    va_start( list, fmt );
    vsprintf( out, fmt, list );
    va_end( list );
    Write(out, strlen(out));
    return true;
}

bool nsStringWriter::Write(const void *data, const uint size) {
    if (!data || size == 0) {
        return false;
    }

    if (_pos + size > _buffer.size()) {
        _buffer.resize(_pos + size, ' ');
    }

    _buffer.replace(_pos, size, static_cast<const char*>(data), size);
    _pos += size;

    return true;
}

bool nsStringWriter::IsValid() {
    return true;
}

bool nsStringWriter::Seek(const long offset, const int origin) {
    if (SEEK_SET == origin) {
        _pos = offset;
    } else if (SEEK_CUR == origin) {
        _pos = _pos + offset;
    } else if (SEEK_END == origin) {
        _pos = _buffer.size() + offset;
    }
    assert(_pos >= 0);
    return true;
}

long nsStringWriter::Tell() {
    return _pos;
}
