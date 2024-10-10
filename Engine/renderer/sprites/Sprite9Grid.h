// Copyright (c) 2003-2024, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file Sprite9Grid.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#include "nsLib/Vec2.h"
#include "nsLib/color.h"
#include "Engine/renderer/TileDef.h"

class nsQuadsBuffer;

struct ITexture;
struct IRenDevice;

class nsSprite9Grid {
public:
    nsColor color;

public:
    virtual ~nsSprite9Grid();

    void SetTexture(ITexture *t);
    ITexture* GetTexture();

    void SetGrid(float leftPad, float topPad, float rightPad, float bottomPad);

    void AddToBuffer(nsQuadsBuffer *buffer);

    void Draw(IRenDevice *dev);

    int GetGridWidth() const;

    void SetGridWidth(int value);

    int GetGridHeight() const;

    void SetGridHeight(int value);

    float GetWidth();
    float GetHeight();

protected:
    ITexture *_tex = nullptr;
    float _leftPad = 0;
    float _rightPad = 0;
    float _topPad = 0;
    float _bottomPad = 0;

    nsTileDef _tiles[3][3];

    int _gridWidth = 3;
    int _gridHeight = 3;

    nsQuadsBuffer *_buff = nullptr;
    bool _invalid = true;

protected:
    int GetTileIndex(int pos, int size);
};