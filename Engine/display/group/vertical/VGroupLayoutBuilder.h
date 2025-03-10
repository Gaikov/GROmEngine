// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VGroupLayoutBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/group/GroupLayoutBuilder.h"

class nsVGroupLayoutBuilder : public nsGroupLayoutBuilder {
public:
    static constexpr auto NAME = "vgroup";

    nsVisualObject2d *CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};