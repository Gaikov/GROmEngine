// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FastAllocatedObject.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "FastMemManager.h"

class nsFastAllocatedObject {
public:
    virtual ~nsFastAllocatedObject() = default;

    void* operator new(size_t size) {
        return nsFastMemManager::Shared()->Alloc(size);
    }

    void operator delete(void* ptr) {
        nsFastMemManager::Shared()->Free(ptr);
    }

    void* operator new[](size_t) = delete;
    void operator delete[](void*) = delete;
};
