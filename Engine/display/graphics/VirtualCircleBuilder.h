// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VirtualCircleBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/factory/VisualBuilder2d.h"

class nsVirtualCircleBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "circle";

    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
    nsVisualObject2d *CreateDefault() override;
};