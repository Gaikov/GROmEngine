//
// Created by Roman on 6/18/2025.
//

#include "FastMemManager.h"
#include "Memory.h"

void * nsFastMemManager::Alloc(const size_t size) const {
    const auto poolIndex = GetPoolIndex(size);
    if (poolIndex < 0) {
        Log::Warning("Allocating big size block: %i", size);
        return my_malloc(size);
    }

    return _pools[poolIndex]->AllocateObject();
}

void nsFastMemManager::Free(void *ptr, const size_t size) const {
    const auto poolIndex = GetPoolIndex(size);
    if (poolIndex < 0) {
        Log::Warning("Freeing big size block: %i", size);
        my_free(ptr);
    }
    _pools[poolIndex]->RecycleObject(ptr);
}

int nsFastMemManager::GetPoolIndex(size_t size) {
    int poolIndex = std::ceil(std::log2(size) - 4);
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
