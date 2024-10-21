// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TextButtonBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "BaseButtonBuilder.h"

class nsTextButtonBuilder : public nsBaseButtonBuilder {
public:
    static constexpr auto NAME = "textButton";

    nsVisualObject2d *CreateDefault() override;

    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
};