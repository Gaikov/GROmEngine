// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualCreationContext.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/Parser.h"
#include "Engine/display/VisualObject2d.h"

class nsVisualBuilder2d;

class nsVisualCreationContext2d {
public:
    virtual nsVisualObject2d *Create(script_state_t *ss) = 0;
    virtual nsVisualObject2d *Create(const char *filePath) = 0;
    virtual nsVisualBuilder2d* GetBuilder(const char *name) = 0;
};