//
// Created by Roman on 6/3/2025.
//

#include "PacketsPool.h"

nsPacketsPool::nsPacketsPool() : nsObjectsPool("net-packets") {
}

nsPacketBuffer * nsPacketsPool::CreateObject() {
    return new nsPacketBuffer();
}

void nsPacketsPool::DestroyObject(nsPacketBuffer *object) {
    delete object;
}

void nsPacketsPool::PrepareObject(nsPacketBuffer *object) {
    memset(object, 0, sizeof(nsPacketBuffer));
}

void nsPacketsPool::OnObjectRecycled(nsPacketBuffer *object) {
}
