// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutsPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/headers.h"
#include "nsLib/SubSystem.h"
#include "VisualPool.h"

class nsLayoutsPool : public nsSubSystem<nsLayoutsPool> {
public:
    void Reserve(const char *path, int amount);
    nsVisualObject2d* Create(const char *path);
    void Recycle(const char *path, nsVisualObject2d *object);

    int GetAllocatedAmount();
    int GetAvailableAmount();

private:
    nsVisualPool::sp_t GetPool(const char *path);

    std::vector<nsVisualPool::sp_t>   _pools;
};