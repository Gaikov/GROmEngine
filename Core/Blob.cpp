//
// Created by Roman on 4/3/2024.
//

#include "Blob.h"
#include "Memory.h"

nsFile::nsFile(uint size) {
    _size = size;
    _data = (uchar *) malloc(size + 1);
    memset(_data, 0, size + 1);
}

nsFile::~nsFile() {
    if (_data) {
        free(_data);
    }
    _data = nullptr;
    _size = 0;
}
