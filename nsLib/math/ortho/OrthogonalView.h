// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OrthogonalView.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/matrix4.h"
#include "engine/UserInput.h"

class nsOrthogonalView : public IPointerTransform {
public:
    virtual const nsMatrix& GetViewMatrix() = 0;
};