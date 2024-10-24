//
// Created by Roman on 10/23/2024.
//

#include "MemoryPool.h"
#include "Core/Memory.h"

nsMemoryPool::nsMemoryPool(int blockSize) :
        _blockSize(blockSize) {
}

nsMemoryPool::~nsMemoryPool() {
    DestroyObjects();
}

void *nsMemoryPool::CreateObject() {
    return my_malloc(_blockSize);
}

void nsMemoryPool::DestroyObject(void *object) {
    my_free(object);
}

void nsMemoryPool::PrepareObject(void *object) {
    memset(object, 0, _blockSize);
}