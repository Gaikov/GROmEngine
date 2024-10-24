// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MemoryPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "ObjectsPool.h"

class nsMemoryPool : public nsObjectsPool<void> {
public:
    explicit nsMemoryPool(int blockSize);
    ~nsMemoryPool() override;

protected:
    void *CreateObject() override;
    void DestroyObject(void *object) override;
    void PrepareObject(void *object) override;

private:
    int _blockSize;
};