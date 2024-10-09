// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OrthogonalFitView.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/matrix4.h"
#include "nsLib/math/ortho/OrthogonalView.h"

class nsOrthogonalFitView : public nsOrthogonalView {
public:
    nsOrthogonalFitView();

    void SetScreenSize(float width, float height);
    void SetTargetSize(float width, float height);
    const nsMatrix& GetViewMatrix() override;
    nsVec2 ScreenToTarget(float x, float y) override;

private:
    mutable nsMatrix    _matrix;
    mutable bool    _valid = false;

    float   _viewWidth = 0;
    float   _viewHeight = 0;

    float   _targetWidth = 0;
    float   _targetHeight = 0;
};