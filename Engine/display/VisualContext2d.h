// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualContext2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/RenDevice.h"
#include "Engine/Font.h"

class nsVisualMaskManager;

class nsVisualContext2d final {
public:
    bool updateLayouts = true;

    virtual ~nsVisualContext2d();
    nsVisualMaskManager* GetMaskManager() const;

private:
    mutable nsVisualMaskManager *_maskManager = nullptr;
};