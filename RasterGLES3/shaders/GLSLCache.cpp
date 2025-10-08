// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GLSLCache.cpp
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#include "GLSLCache.h"

void nsGLSLCache::Invalidate() const {
    for (const auto &item : _cache) {
        item.second.item->Invalidate();
    }
}

nsGLSLCode *nsGLSLCache::AllocateResource(const char *resourceName, int /*param*/) {
    return new nsGLSLCode(resourceName);
}

void nsGLSLCache::FreeResource(nsGLSLCode *item) {
    delete item;
}
