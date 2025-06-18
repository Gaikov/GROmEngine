//
// Created by Roman on 6/18/2025.
//
#include "UnitCommon.h"
#include "Core/memory/FastMemManager.h"

TEST(FastMem, PoolIndex) {
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(20) == 1);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(600) == -1);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(1) == 0);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(50) == 2);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(16) == 0);
}