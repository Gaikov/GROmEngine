// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file MovieClipBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/factory/VisualBuilder2d.h"

class nsMovieClipBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "movieClip";

    nsVisualObject2d* CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};
