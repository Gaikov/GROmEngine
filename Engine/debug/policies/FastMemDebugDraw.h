// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FastMemDebugDraw.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once
#include "debug/DebugDrawPolicy.h"

class nsFastMemDebugDraw : public nsDebugDrawPolicy {
public:
    void Draw() override;
    nsFastMemDebugDraw();
};
