// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FastAllocatedObject.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "FastMemManager.h"

class nsFastAllocatedObject {
public:
    void* operator new(size_t size) {
        return nsFastMemManager::Shared()->Alloc(size);
    }

    void operator delete(void* ptr, size_t size) {
        nsFastMemManager::Shared()->Free(ptr, size);
    }

    void operator delete(void*) = delete;
    void* operator new[](size_t) = delete;
    void operator delete[](void*) = delete;
};
