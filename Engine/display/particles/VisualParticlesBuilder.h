// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualParticlesBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/factory/VisualBuilder2d.h"

class nsVisualParticlesBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "particles";

    nsVisualObject2d *Create(script_state_t *ss, nsVisualCreationContext2d *context) override;
    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
};