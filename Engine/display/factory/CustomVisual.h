// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CustomVisual.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Core/Parser.h"

class nsCustomVisual {
public:
    virtual ~nsCustomVisual() = default;
    virtual bool ParseCustomProps(script_state_t *ss) = 0;
};