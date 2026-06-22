// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualPolygonBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/display/VisualType.h"
#include "Engine/display/factory/VisualBuilder2d.h"

class nsVisualPolygonBuilder : public nsVisualBuilder2d {
public:
    static constexpr auto NAME = nsVisualType::POLYGON;

    nsVisualObject2d *CreateDefault(nsVisualCreationContext2d *context) override;
    bool Parse(script_state_t *ss, nsVisualObject2d *o, nsVisualCreationContext2d *context) override;

protected:
    bool SerializeProps(nsScriptSaver &saver, nsVisualObject2d *o,
                        nsVisualCreationContext2d *context) override;
};
