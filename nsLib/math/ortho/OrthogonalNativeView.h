// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file OrthogonalNativeView.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "OrthogonalView.h"

class nsOrthogonalNativeView : public nsOrthogonalView {
public:
    void SetScreenSize(float width, float height);
    const nsMatrix &GetViewMatrix() override;
    nsVec2 ScreenToTarget(float x, float y) override;

protected:
    float       _width = 1;
    float       _height = 1;
    bool        _valid = false;
    nsMatrix    _matrix;
};