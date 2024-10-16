// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextLabelBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/factory/VisualBuilder2d.h"

class nsTextLabelBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "label";

    nsVisualObject2d *CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};