// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file SpriteDesc.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/comm_types.h"
#include "nsLib/Vec2.h"
#include "nsLib/color.h"
#include "nsLib/BBox2.h"
#include "engine/RenDevice.h"
#include "engine/renderer/QuadsBuffer.h"

struct nsSpriteDesc
{
    inline nsSpriteDesc() :
            tex(nullptr),
            tex2(1, 1) {}

	ITexture	*tex;
	nsVec2		pos;
	nsVec2		center;
	nsVec2		size;
	nsColor		color;
	nsVec2		tex1, tex2;

    nsSpriteDesc& ResetSize();
    nsSpriteDesc& ComputeCenter();

    void Draw(IRenDevice *dev) const;

private:
    static nsQuadsBuffer    *_buff;
};