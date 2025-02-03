// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImageButton.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/VisualObject2d.h"
#include "input/InteractiveElement.h"
#include "renderer/sprites/SpriteDesc.h"
#include "BaseButton.h"

class nsImageButton : public nsVisualObject2d, public nsInteractiveElement {
public:
    nsImageButton();

    nsBaseButton::handler_t onClick;
    IRenState       *renState = nullptr;
    nsSpriteDesc    up, over, down, disabled;

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
    bool HitTest(float x, float y) override;

protected:
    nsSpriteDesc    *_currentState;

    void OnPointerOver() override;
    void OnPointerOut() override;
    void OnPointerDown() override;
    void OnPointerUp() override;
    void OnClick() override;
};