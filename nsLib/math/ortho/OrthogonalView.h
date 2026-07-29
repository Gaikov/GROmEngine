// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OrthogonalView.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/matrix4.h"
#include "Engine/UserInput.h"

class nsOrthogonalView : public IPointerTransform {
public:
    virtual void SetScreenSize(float width, float height) = 0;
    virtual const nsMatrix& GetViewMatrix() = 0;
};
