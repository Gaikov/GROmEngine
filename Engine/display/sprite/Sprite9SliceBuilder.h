// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9SliceBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/factory/VisualBuilder2d.h"

class nsSprite9SliceBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "9slice";

    nsVisualObject2d *CreateDefault() override;

    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
};