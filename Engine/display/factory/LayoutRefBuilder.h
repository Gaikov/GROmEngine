// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LayoutRefBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "VisualBuilder2d.h"
#include "Engine/display/VisualType.h"

class nsLayoutRefBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = nsVisualType::LAYOUT_REF;

    nsVisualObject2d *Create(script_state_t *ss, nsVisualCreationContext2d *context) override;
    nsVisualObject2d *CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};
