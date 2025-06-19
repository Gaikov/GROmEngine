//
// Created by Roman on 6/18/2025.
//
#include "Core/debug/LogStdOut.h"

#include "UnitCommon.h"
#include "Core/memory/FastMemManager.h"
#include "Core/memory/FastMemTest.h"

static LogStdOut   out;

static void MemInit() {
    Log::Init();
    Log::Shared()->AddPolicy(&out);
    nsFastMemManager::Init();
}

static void MemRelease() {
    nsFastMemManager::Release();
    Log::Release();
}

TEST(FastMem, PoolIndex) {
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(20) == 1);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(600) == -1);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(1) == 0);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(50) == 2);
    ASSERT_TRUE(nsFastMemManager::GetPoolIndex(16) == 0);
}

TEST(FastMem, AllocPeformance) {
    MemInit();

    nsFastMemTest::Run();

    MemRelease();
}