// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9SliceBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/factory/VisualBuilder2d.h"

class nsSprite9SliceBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = nsVisualType::SPRITE9_SLICE;

    nsVisualObject2d *CreateDefault() override;

    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
};