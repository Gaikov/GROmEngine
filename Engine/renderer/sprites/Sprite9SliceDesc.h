// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9Slice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/color.h"
#include "nsLib/Vec2.h"
#include "Engine/RenDevice.h"

class nsQuadsBuffer;
struct ITexture;

class nsSprite9SliceDesc final {
public:
    nsColor  color;  // applied via SetColor each draw — does not invalidate the buffer

    virtual ~nsSprite9SliceDesc();

    // Setters that mark the quad buffer dirty when the value actually changes.
    void SetTex(ITexture *t);                          // texture (UV depends on its size)
    void SetAtlasRegion(nsVec2 t1, nsVec2 t2);        // pixel sub-region in atlas
    void SetGrid(float xMinPad, float xMaxPad, float yMinPad, float yMaxPad);
    void SetWidth(float width);
    void SetHeight(float height);

    ITexture *GetTex() const { return _tex; }
    float GetWidth()  const { return _width; }
    float GetHeight() const { return _height; }

    void AddToBuffer(nsQuadsBuffer *buffer);
    void Draw(IRenDevice *dev);

private:
    ITexture        *_tex   = nullptr;
    nsVec2          _tex1Px = {0, 0};  // pixel offset in atlas, {0,0} = start of texture
    nsVec2          _tex2Px = {0, 0};  // pixel end in atlas,    {0,0} = full texture (backward compat)

    IRenDevice      *_device = nullptr;
    nsQuadsBuffer   *_buff   = nullptr;
    bool             _invalid = true;

    float _xMinPad = 0;
    float _xMaxPad = 0;
    float _yMinPad = 0;
    float _yMaxPad = 0;

    float _width  = 100;
    float _height = 100;
};