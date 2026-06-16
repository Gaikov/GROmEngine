// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImageButtonBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/factory/VisualBuilder2d.h"
#include "renderer/sprites/SpriteDesc.h"

class nsImageButtonBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = nsVisualType::IMAGE_BUTTON;

    nsVisualObject2d *CreateDefault(nsVisualCreationContext2d *context) override;

private:
    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;

};