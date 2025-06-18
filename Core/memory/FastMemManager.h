// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FastMemManager.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "MemoryPool.h"
#include "nsLib/SubSystem.h"

class nsFastMemManager : public nsSubSystem<nsFastMemManager> {
public:
    void* Alloc(size_t size) const;
    void Free(void* ptr, size_t size) const;
    static int GetPoolIndex(size_t size);

protected:
    bool OnInit() override;
    void OnRelease() override;

private:
    std::vector<nsMemoryPool*>   _pools;
};
