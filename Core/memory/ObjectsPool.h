// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ObjectsPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "nsLib/log.h"

template <class TClass>
class nsObjectsPool {
public:
    explicit nsObjectsPool() = default;
    virtual ~nsObjectsPool() = default;

    void DestroyObjects() {
        Log::Info("releasing objects pool of %i items", (int)_allocated.size());

        for (auto object : _allocated) {
            DestroyObject(object);
        }
        _allocated.clear();
        _pool.clear();
    }

    TClass* AllocateObject() {
        if (_pool.empty()) {
            auto amount = (int)_allocated.size();
            if (!amount) {
                amount = 1;
            }
            Reserve(amount);
        }

        auto object = _pool[_pool.size() - 1];
        _pool.pop_back();

        PrepareObject(object);
        return object;
    }

    void RecycleObject(TClass *object) {
        //TODO: check object already in pool and allocated by the pool
        _pool.push_back(object);
    }

    void Reserve(int numObjects) {
        for (auto i = 0; i < numObjects; i++) {
            auto object = CreateObject();
            _allocated.push_back(object);
            _pool.push_back(object);
        }

        Log::Info("objects reserved: %i/%i", numObjects, (int)_allocated.size());
    }

    nsObjectsPool<TClass>& operator = (const nsObjectsPool<TClass> &other) = delete;
    nsObjectsPool(const nsObjectsPool<TClass> &other) = delete;
    bool operator == (const nsObjectsPool<TClass> &other) = delete;
    bool operator != (const nsObjectsPool<TClass> &other) = delete;

protected:
    virtual TClass* CreateObject() = 0;
    virtual void DestroyObject(TClass *object) = 0;
    virtual void PrepareObject(TClass *object) = 0;

private:
    std::vector<TClass*>  _pool;
    std::vector<TClass*>  _allocated;
};