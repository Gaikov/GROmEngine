// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GroupLayoutBuilder.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "display/container/VisualContainerBuilder.h"

class nsGroupLayoutBuilder : public nsVisualContainerBuilder {
public:
    static constexpr auto NAME = nsVisualType::GROUP;

    nsVisualObject2d *CreateDefault() override;
    bool Parse(script_state_t *ss, nsVisualObject2d *object, nsVisualCreationContext2d *context) override;
};