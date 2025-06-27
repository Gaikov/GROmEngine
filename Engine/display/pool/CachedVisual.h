// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CachedVisual.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

class nsCachedVisual {
public:
    virtual ~nsCachedVisual() = default;
    virtual void OnVisualCreated(const char *path) = 0;
    virtual void OnVisualPrepare() = 0;
    virtual void OnVisualRecycled() = 0;
};