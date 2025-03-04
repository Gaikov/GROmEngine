// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImageButton.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Engine/input/InteractiveElement.h"
#include "Engine/renderer/sprites/SpriteDesc.h"
#include "BaseButton.h"
#include "Engine/display/align/Align.h"

class nsImageButton : public nsVisualObject2d, public nsInteractiveElement {
public:
    nsImageButton();

    nsBaseButton::handler_t onClick;
    IRenState       *renState = nullptr;
    nsSpriteDesc    up, over, down, disabled;

    nsString    text;
    nsFont      *font = nullptr;
    nsVec2      labelPos;

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
    bool HitTest(float x, float y) override;
    void AlignText(nsAlign::Type hAlign, nsAlign::Type vAlign);

protected:
    nsSpriteDesc    *_currentState;

    void OnPointerOver() override;
    void OnPointerOut() override;
    void OnPointerDown() override;
    void OnPointerUp() override;
    void OnClick() override;
};