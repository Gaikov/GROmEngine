//
// Created by Roman on 4/3/2024.
//

#include "Blob.h"
#include "Memory.h"

nsFile::nsFile(uint size) {
    assert(size != 0);
    m_size = size;
    m_data = (uchar *) malloc(size + 1);
    memset(m_data, 0, size + 1);
}

nsFile::~nsFile() {
    if (m_data) {
        free(m_data);
    }
    m_data = nullptr;
    m_size = 0;
}
