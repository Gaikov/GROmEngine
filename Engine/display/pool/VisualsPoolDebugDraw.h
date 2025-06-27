// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file GameDebugInfo.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Engine/debug/DebugDrawPolicy.h"

class nsVisualsPoolDebugDraw : public nsDebugDrawPolicy {
public:
    nsVisualsPoolDebugDraw();
    void Draw() override;
};