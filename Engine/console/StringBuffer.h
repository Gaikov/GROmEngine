// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StringBuffer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "nsLib/utils/NoCopyable.h"

class nsStringBuffer final : public nsNoCopyable {
public:
    static nsStringBuffer* Create(const int maxSize) {
        return new nsStringBuffer(maxSize);
    }
    ~nsStringBuffer() override;
    char* Allocate(int size);

private:
    explicit nsStringBuffer(int maxSize);

    char *_buffer;
    int _size;
    int _pos = 0;
};
