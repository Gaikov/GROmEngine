//
// Created by Roman on 10/24/2024.
//

#include "VisualPool.h"
#include "display/factory/VisualFactory2d.h"
#include "CachedVisual.h"

nsVisualPool::nsVisualPool(const char *path) :
        nsObjectsPool(path),
        _path(path) {
}

nsVisualPool::~nsVisualPool() {
    DestroyObjects();
}

nsVisualObject2d *nsVisualPool::CreateObject() {
    auto obj = nsVisualFactory2d::Shared()->Create(_path.c_str());
    if (auto v = dynamic_cast<nsCachedVisual*>(obj)) {
        v->OnVisualCreated(_path.c_str());
    }
    return obj;
}

void nsVisualPool::DestroyObject(nsVisualObject2d *object) {
    object->Destroy();
}

void nsVisualPool::PrepareObject(nsVisualObject2d *object) {
    if (auto v = dynamic_cast<nsCachedVisual*>(object)) {
        v->OnVisualPrepare();
    }
}

const std::string &nsVisualPool::GetPath() {
    return _path;
}

void nsVisualPool::OnObjectRecycled(nsVisualObject2d *object) {
    if (auto v = dynamic_cast<nsCachedVisual*>(object)) {
        v->OnVisualRecycled();
    }
}


