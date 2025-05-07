// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextAreaBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "display/factory/VisualBuilder2d.h"

class nsTextAreaBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "textArea";

    nsVisualObject2d *CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};
