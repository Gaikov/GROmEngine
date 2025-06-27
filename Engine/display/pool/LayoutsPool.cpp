//
// Created by Roman on 10/25/2024.
//

#include "LayoutsPool.h"
#include "display/container/VisualContainer2d.h"

nsVisualObject2d *nsLayoutsPool::Create(const char *path) {
    assert(StrCheck(path));

    return GetPool(path)->AllocateObject();
}

void nsLayoutsPool::Recycle(const char *path, nsVisualObject2d *object) {
    assert(StrCheck(path) && object);

    auto parent = object->GetParent();
    if (parent) {
        parent->RemoveChild(object);
    }

    GetPool(path)->RecycleObject(object);
}

nsVisualPool::sp_t nsLayoutsPool::GetPool(const char *path) {
    for (auto &pool : _pools) {
        if (pool->GetPath() == path) {
            return pool;
        }
    }

    nsVisualPool::sp_t pool = new nsVisualPool(path);
    _pools.push_back(pool);
    Log::Info("layouts pool allocated: %s", path);

    return pool;
}

void nsLayoutsPool::Reserve(const char *path, int amount) {
    GetPool(path)->Reserve(amount);
}

int nsLayoutsPool::GetAllocatedAmount() {
    int amount = 0;
    for (auto &p : _pools) {
        amount += p->GetAllocatedAmount();
    }
    return amount;
}

int nsLayoutsPool::GetAvailableAmount() {
    int amount = 0;
    for (auto &p : _pools) {
        amount += p->GetAvailableAmount();
    }
    return amount;
}
