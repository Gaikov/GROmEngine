// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScrollThumb.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/sprite/Sprite9Slice.h"
#include "input/InteractiveElement.h"

class nsScrollThumb : public nsSprite9Slice, public nsInteractiveElement {
public:
    nsColor     colorOut = nsColor(0.15, 0.15, 0.15, 1);
    nsColor     colorOver = nsColor(0.2, 0.2, 0.2, 1);
    nsColor     colorDown = nsColor(0.3, 0.3, 0.3, 1);

    nsScrollThumb();
    bool HitTest(float x, float y) override;

protected:
    bool _over = false;
    bool _down = false;

    void UpdateState();

    void OnPointerOver() override;
    void OnPointerOut() override;
    void OnPointerDown() override;
    void OnPointerUp() override;
    void OnClick() override;
};