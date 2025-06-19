//
// Created by Roman on 6/18/2025.
//

#include "FastMemManager.h"
#include "Memory.h"

#define BLOCK_ID 0xFF1234CC

void * nsFastMemManager::Alloc(size_t size) const {
    size += sizeof(BlockHeader);

    const auto poolIndex = GetPoolIndex(size);
    char *block;
    if (poolIndex < 0) {
        Log::Warning("Allocating big size block: %i", size);
        block = static_cast<char *>(my_malloc(size));
    } else {
        block = static_cast<char *>(_pools[poolIndex]->AllocateObject());
    }

    auto header = reinterpret_cast<BlockHeader *>(block);
    header->size = size;
    header->poolIndex = poolIndex;
    header->id = BLOCK_ID;

    return block + sizeof(BlockHeader);
}

bool nsFastMemManager::Free(void *ptr) const {
    char *block = static_cast<char *>(ptr) - sizeof(BlockHeader);
    const auto header = reinterpret_cast<BlockHeader *>(block);
    if (header->id != BLOCK_ID) {
        Log::Error("Invalid memory block!", ptr);
        return false;
    }

    const auto poolIndex = GetPoolIndex(header->size);
    if (header->poolIndex != poolIndex) {
        Log::Error("Invalid memory block pool index!", ptr);
        return false;
    }

    if (poolIndex < 0) {
        Log::Warning("Freeing big size block: %i", header->size);
        my_free(block);
    } else {
        _pools[poolIndex]->RecycleObject(block);
    }
    return true;
}

void nsFastMemManager::Reserve(const size_t size, const int amount) const {
    auto poolIndex = GetPoolIndex(size);
    if (poolIndex < 0) {
        Log::Warning("Can't reserve big size block: %i", size);
    } else {
        _pools[poolIndex]->Reserve(amount);
    }
}

int nsFastMemManager::GetPoolIndex(size_t size) {

    int poolIndex = ceil(log2(static_cast<float>(size)) - 4);
    if (poolIndex < 0) {
        poolIndex = 0;
    } else if (poolIndex > 5) {
        poolIndex = -1;
    }
    return poolIndex;
}

void nsFastMemManager::GetPoolInfo(int index, PoolInfo &info) const {
    auto p = _pools[index];
    info.debugName = p->GetName();
    info.usedBlocks = p->GetUsedAmount();
    info.totalBlocks = p->GetAllocatedAmount();
}

bool nsFastMemManager::OnInit() {
    if (!nsSubSystem::OnInit()) {
        return false;
    }

    for (int i = 16; i <= 512; i *= 2) {
        std::string name = "mem_pool_" + std::to_string(i);
        _pools.push_back(new nsMemoryPool(name.c_str(), i));
    }

    for (auto p : _pools) {
        p->Reserve(100);
    }

    Log::Info("...memory pool initialized");
    return true;
}

void nsFastMemManager::OnRelease() {
    Log::Info("...releasing memory pools");

    for (const auto p : _pools) {
        const auto amount = p->GetUsedAmount();
        if (amount > 0) {
            Log::Warning("pool %s has allocations: %i", p->GetName(), amount);
        }
        delete p;
    }

    nsSubSystem::OnRelease();
}
