// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9Slice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/VisualObject2d.h"
#include "Engine/renderer/sprites/Sprite9SliceDesc.h"

class nsSprite9Slice : public nsVisualObject2d {
public:
    nsSprite9SliceDesc desc;
    IRenState           *renState = nullptr;

    void SetWidth(float width) override;
    void SetHeight(float height) override;

    const char * GetType() override { return nsVisualType::SPRITE9_SLICE; }

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;
};