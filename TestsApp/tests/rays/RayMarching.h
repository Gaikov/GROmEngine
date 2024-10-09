// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file RayMarching.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"

class nsSDFunction {
public:
    virtual float GetDistance(const nsVec2 &point) const = 0;
};

class nsBoxFunction : public nsSDFunction {
public:
    nsBoxFunction(float width, float height);
    float GetDistance(const nsVec2 &point) const override;

private:
    float _width;
    float _height;
};

class nsRayMarching final {
public:
    static float GetDistance(const nsSDFunction *func, const nsVec2 &start, const nsVec2 &dir, float maxDistance, int numSteps = 100);
};