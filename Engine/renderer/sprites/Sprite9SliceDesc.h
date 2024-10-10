// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9Slice.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/color.h"
#include "Engine/RenDevice.h"

class nsQuadsBuffer;
struct ITexture;

class nsSprite9SliceDesc final {
public:
    ITexture        *tex = nullptr;
    nsColor         color;

    virtual ~nsSprite9SliceDesc();

    void SetGrid(float xMinPad, float xMaxPad, float yMinPad, float yMaxPad);

    void AddToBuffer(nsQuadsBuffer *buffer);
    void Draw(IRenDevice *dev);

    void SetWidth(float width);
    void SetHeight(float height);
    float GetWidth() const {return _width;}
    float GetHeight() const {return _height;}

private:
    IRenDevice      *_device = nullptr;
    nsQuadsBuffer   *_buff = nullptr;
    bool _invalid = true;

    float _xMinPad = 0;
    float _xMaxPad = 0;
    float _yMinPad = 0;
    float _yMaxPad = 0;

    float _width = 100;
    float _height = 100;
};