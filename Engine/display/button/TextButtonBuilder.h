// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextButtonBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "BaseButtonBuilder.h"
#include "Engine/display/VisualType.h"

class nsTextButtonBuilder : public nsBaseButtonBuilder {
public:
    static constexpr auto NAME = nsVisualType::TEXT_BUTTON;

    nsVisualObject2d *CreateDefault() override;

    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
};
