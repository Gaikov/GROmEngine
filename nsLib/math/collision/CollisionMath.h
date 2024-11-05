// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file CollisionMath.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/math/Transform2.h"
#include "nsLib/math/Rect.h"

struct nsCollisionMath final {
    static bool RectToRect(nsTransform2 &t1, const nsRect &r1, nsTransform2 &t2, const nsRect &r2);
};