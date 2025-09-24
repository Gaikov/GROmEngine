// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ConsoleBuffer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include "StringBuffer.h"

class nsTextBuffer final : public nsNoCopyable {
public:
    explicit nsTextBuffer(int maxChunkSize);
    ~nsTextBuffer() override;

    char* Allocate(int size);

private:
    std::vector<nsStringBuffer*> _buffers;
    nsStringBuffer *_currentBuffer = nullptr;
    int _maxChunkSize;
};
