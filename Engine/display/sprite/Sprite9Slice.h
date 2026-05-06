// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9Slice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/VisualObject2d.h"
#include "Engine/renderer/sprites/Sprite9SliceDesc.h"
#include "nsLib/models/Property.h"
#include "nsLib/color.h"

class nsSprite9Slice : public nsVisualObject2d {
public:
    nsProperty<ITexture*>   tex;
    nsProperty<float>       width;
    nsProperty<float>       height;

    // Atlas sub-region in pixels. tex2 = {0,0} means full texture (backward compat).
    nsProperty<nsVec2>  tex1;
    nsProperty<nsVec2>  tex2;

    // 9-slice grid padding in pixels from each edge of the frame.
    nsProperty<float>   gridLeft;
    nsProperty<float>   gridRight;
    nsProperty<float>   gridTop;
    nsProperty<float>   gridBottom;

    nsColor     color     = nsColor::white;
    IRenState   *renState = nullptr;

    nsSprite9Slice();

    // Convenience: set all four grid paddings at once.
    void SetGrid(float left, float right, float top, float bottom);

    // Overrides convert world size → logical size and write to width/height properties.
    void SetWidth(float worldWidth) override;
    void SetHeight(float worldHeight) override;

    const char * GetType() override { return nsVisualType::SPRITE9_SLICE; }

    void GetLocalBounds(nsRect &bounds) override;
    void Loop() override;
    void DrawContent(const nsVisualContext2d &context) override;

private:
    nsSprite9SliceDesc _desc;
    const nsEventDispatcher::tEventHandler _onChangeHandler;

    void _SyncDesc();
};