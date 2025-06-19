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
    struct PoolInfo {
        const char *debugName;
        int usedBlocks;
        int totalBlocks;
    };

    void* Alloc(size_t size) const;
    bool Free(void* ptr) const;
    void Reserve(size_t size, int amount) const;
    static int GetPoolIndex(size_t size);

    [[nodiscard]] size_t GetPoolsAmount() const { return _pools.size(); }
    inline void GetPoolInfo(int index, PoolInfo &info) const;

protected:
    struct BlockHeader {
        uint32_t id;
        unsigned char poolIndex;
        size_t size;
    };

    bool OnInit() override;
    void OnRelease() override;

private:
    std::vector<nsMemoryPool*>   _pools;
};
