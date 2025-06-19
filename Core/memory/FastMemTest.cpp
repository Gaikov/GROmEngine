//
// Created by Roman on 6/19/2025.
//

#include "FastMemTest.h"
#include "Profiler.h"
#include "FastAllocatedObject.h"
#include "FastMemManager.h"

#define MAX_OBJECTS 1000000
#define BLOCK_SIZE 400

class FastClass : public nsFastAllocatedObject {
    char ch[BLOCK_SIZE] = {};
};

class SlowClass {
    char ch[BLOCK_SIZE] = {};
};

void nsFastMemTest::Run() {
    nsFastMemManager::Shared()->Reserve(BLOCK_SIZE, MAX_OBJECTS);

    Log::Info("Starting test...");

    std::vector<FastClass*> fastObjects;
    fastObjects.reserve(MAX_OBJECTS);

    {
        nsProfilerSample    fast("fast objects allocated");
        for (int i = 0; i < MAX_OBJECTS; i++) {
            fastObjects.push_back(new FastClass());
        }
    }

    {
        nsProfilerSample    fast("fast objects freed");
        for (auto o : fastObjects) {
            delete o;
        }
    }

    std::vector<SlowClass*> slowObjects;

    {
        nsProfilerSample    slow("slow objects allocated");
        for (int i = 0; i < MAX_OBJECTS; i++) {
            slowObjects.push_back(new SlowClass());
        }
    }

    {
        nsProfilerSample    slow("slow objects freed");
        for (auto o : slowObjects) {
            delete o;
        }
    }
}
