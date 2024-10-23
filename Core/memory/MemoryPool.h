// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MemoryPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"

class nsMemoryPool {
public:
    explicit nsMemoryPool(int blockSize);
    virtual ~nsMemoryPool();

    void* Allocate();
    void Free(void *data);

private:
    int _blockSize;

    std::vector<void*>  _pool;
    std::vector<void*>  _allocated;

    void Reserve(int numBlocks);
};