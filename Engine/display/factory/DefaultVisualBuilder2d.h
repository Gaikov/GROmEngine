// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DefaultVisualBuilder2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "VisualBuilder2d.h"

class nsDefaultVisualBuilder2d : public nsVisualBuilder2d {
public:
    nsVisualObject2d * CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;

};
