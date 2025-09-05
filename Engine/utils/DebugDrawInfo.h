// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file DebugDrawInfo.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"
#include "nsLib/color.h"

class nsDebugDrawInfo {
public:
    explicit nsDebugDrawInfo(const nsVec2 &startPos);
    void DrawLine(const char *text, const nsColor &c = nsColor::white);
    void DrawLine(const nsVec2 &pos, const char *text, const nsColor &c = nsColor::white);

private:
    nsVec2  _pos;
};