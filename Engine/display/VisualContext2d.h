// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualContext2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "engine/RenDevice.h"
#include "engine/Font.h"

class nsVisualMaskManager;

class nsVisualContext2d final {
public:
    bool updateLayouts = true;

    virtual ~nsVisualContext2d();
    nsVisualMaskManager* GetMaskManager() const;

private:
    mutable nsVisualMaskManager *_maskManager = nullptr;
};