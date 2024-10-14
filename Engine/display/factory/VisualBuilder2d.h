// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file VisualBuilder2d.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/Parser.h"
#include "Engine/display/VisualObject2d.h"
#include "Core/SmartPtr.h"
#include "VisualCreationContext.h"

class nsVisualBuilder2d {
public:
    typedef nsSmartPtr<nsVisualBuilder2d>   sp_t;
    virtual ~nsVisualBuilder2d() = default;

    virtual nsVisualObject2d* Create(script_state_t *ss, nsVisualCreationContext2d *context) = 0;
    virtual bool Parse(script_state_t *ss, nsVisualObject2d *object);
};