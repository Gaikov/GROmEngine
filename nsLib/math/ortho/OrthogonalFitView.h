// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OrthogonalFitView.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/math/Rect.h"
#include "nsLib/matrix4.h"
#include "nsLib/math/ortho/OrthogonalView.h"

class nsOrthogonalFitView : public nsOrthogonalView {
public:
    nsOrthogonalFitView();

    void SetScreenSize(float width, float height);
    void SetTargetSize(float width, float height);
    const nsMatrix& GetViewMatrix() override;
    nsVec2 ScreenToTarget(float x, float y) override;
    const nsRect& GetTargetViewport() const {
        return _targetViewport;
    }

protected:
    void ComputeTargetViewport();

private:
    mutable nsMatrix    _matrix;
    mutable bool    _valid = false;

    float   _screenWidth = 0;
    float   _screenHeight = 0;

    float   _targetWidth = 0;
    float   _targetHeight = 0;

    nsRect _targetViewport;
};