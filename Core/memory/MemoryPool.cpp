//
// Created by Roman on 10/23/2024.
//

#include "MemoryPool.h"
#include "Core/Memory.h"
#include "nsLib/log.h"

nsMemoryPool::nsMemoryPool(int blockSize) : _blockSize(blockSize) {
    Reserve(10);
}

nsMemoryPool::~nsMemoryPool() {
    Log::Info("releasing memory pool %i blocks", (int)_allocated.size());

    for (auto block : _allocated) {
        my_free(block);
    }
}

void nsMemoryPool::Reserve(int numBlocks) {
    for (auto i = 0; i < numBlocks; i++) {
        auto block = my_malloc(_blockSize);
        _allocated.push_back(block);
        _pool.push_back(block);
    }

    Log::Info("memory blocks reserved: %i/%i", numBlocks, (int)_allocated.size());
}

void *nsMemoryPool::Allocate() {
    if (_pool.empty()) {
        Reserve((int)_allocated.size());
    }

    auto block = _pool[_pool.size() - 1];
    _pool.pop_back();

    memset(block, 0, _blockSize);
    return block;
}

void nsMemoryPool::Free(void *data) {
    _pool.push_back(data);
}
