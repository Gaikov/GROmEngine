// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualObject2d.h"
#include "Engine/display/VisualType.h"
#include "Engine/renderer/sprites/SpriteDesc.h"

class nsSprite : public nsVisualMask {
public:
    nsSpriteDesc    desc;
    IRenState       *renState = nullptr;
    bool            drawFrame = false;

    nsSprite();
    const char * GetType() override { return nsVisualType::SPRITE; }
    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;

protected:
    void DrawContent(const nsVisualContext2d &context) override;
};
