// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TriMath.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "Vec2.h"

class nsTriMath {
public:
    static bool				PointInTriangle( const nsVec2 &pt, const nsVec2 &v1, const nsVec2 &v2, const nsVec2 &v3 );
};