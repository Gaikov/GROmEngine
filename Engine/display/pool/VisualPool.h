// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualPool.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Core/memory/ObjectsPool.h"
#include "Core/SmartPtr.h"

class nsVisualPool : public nsObjectsPool<nsVisualObject2d> {
public:
    typedef nsSmartPtr<nsVisualPool>    sp_t;

    const std::string& GetPath();
    explicit nsVisualPool(const char *path);
    ~nsVisualPool() override;

protected:
    nsVisualObject2d *CreateObject() override;
    void DestroyObject(nsVisualObject2d *object) override;
    void PrepareObject(nsVisualObject2d *object) override;
    void OnObjectRecycled(nsVisualObject2d *object) override;

private:
    std::string _path;
};