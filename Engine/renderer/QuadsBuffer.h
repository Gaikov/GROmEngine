// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file QuadsBuffer.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/color.h"
#include "TileDef.h"

struct IRenDevice;
struct IVertexBuffer;

class nsQuadsBuffer {
public:
    nsQuadsBuffer(IRenDevice *dev, uint maxQuads, bool useColors = false);

    bool AddQuad(const nsVec2 &pos, const nsVec2 &pivot, const nsVec2 &size, const nsColor &c, const nsVec2 &uvStart, const nsVec2 &uvEnd, float angle = 0);

    inline bool AddQuad(const nsVec2 &pos, const nsTileDef &tile, const nsColor &c) {
        return AddQuad(pos, {}, tile.size, c, tile.uvStart, tile.uvEnd);
    }
    inline bool AddQuad(const nsVec2 &pos, nsVec2 &pivot, float angle, const nsTileDef &tile, const nsColor &c) {
        return AddQuad(pos, pivot, tile.size, c, tile.uvStart, tile.uvEnd, angle);
    }

    void Draw();
    void Clear();

private:
    IRenDevice *_dev;
    IVertexBuffer *_vb;
    int _numQuads;
    uint _maxQuads;
};