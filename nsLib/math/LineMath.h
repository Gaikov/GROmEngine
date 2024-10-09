// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file LineMath.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Vec2.h"

class nsLineMath {
public:
    static float			PointOnLine( const nsVec2 &in, nsVec2 &out, const nsVec2 &p1, const nsVec2 &p2 );
};