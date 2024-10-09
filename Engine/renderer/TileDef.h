// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file TileDef.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"

struct ITexture;

class nsTileDef {
public:
    nsVec2 size;
    nsVec2 uvStart;
    nsVec2 uvEnd;

public:
    void Compute(ITexture *tex, const nsVec2 &texturePos, const nsVec2 &rectSize);
    void Compute(ITexture *tex, float x, float y, float width, float height);
    void Compute(ITexture *tex);
};