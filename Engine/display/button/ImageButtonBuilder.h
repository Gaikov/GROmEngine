// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ImageButtonBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/factory/VisualBuilder2d.h"
#include "renderer/sprites/SpriteDesc.h"

class nsImageButtonBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = "imageButton";

    nsVisualObject2d *CreateDefault() override;

private:
    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;
    void ParseState(script_state_t *ss, const char *name, nsSpriteDesc &desc);

};