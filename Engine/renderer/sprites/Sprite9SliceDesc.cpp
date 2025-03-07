//
// Created by Roman on 9/16/2024.
//

#include "Sprite9SliceDesc.h"
#include "renderer/QuadsBuffer.h"
#include "nsLib/log.h"

nsSprite9SliceDesc::~nsSprite9SliceDesc() {
    delete _buff;
}

void nsSprite9SliceDesc::SetGrid(float xMinPad, float xMaxPad, float yMinPad, float yMaxPad) {
    _xMinPad = xMinPad;
    _xMaxPad = xMaxPad;
    _yMinPad = yMinPad;
    _yMaxPad = yMaxPad;
    _invalid = false;
}

void nsSprite9SliceDesc::AddToBuffer(nsQuadsBuffer *buffer) {
    if (!tex) {
        Log::Warning("Texture is not set for Sprite9Grid!");
        return;
    }

    int w, h;
    tex->GetSize(w, h);

    auto texWidth = (float) w;
    auto texHeight = (float) h;

    auto midHorz = texWidth - (_xMinPad + _xMaxPad);
    auto midVert = texHeight - (_yMinPad + _yMaxPad);
    auto right = texWidth - _xMaxPad;
    auto bottom = texHeight - _yMaxPad;
    auto left = _xMinPad;
    auto top = _yMinPad;

    float posX[] = {0, left, right};
    float sizeX[] = {_xMinPad, midHorz, _xMaxPad};
    float posY[] = {bottom, top, 0};
    float sizeY[] = {_yMaxPad, midVert, _yMinPad};

    nsTileDef   tiles[3][3];
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            tiles[x][y].Compute(tex, posX[x], posY[y], sizeX[x], sizeY[y]);
        }
    }

    left = _xMinPad;
    right = _xMaxPad;
    midHorz = _width - (_xMinPad + _xMaxPad);
    if (midHorz < 0) {
        left = right = std::max(_width + midHorz, 0.0f) / 2.0f;
        midHorz = 0;
    }
    float tileSizeX[] = {left, midHorz, right};

    bottom = _yMaxPad;
    top = _yMinPad;
    midVert = _height - (_yMinPad + _yMaxPad);
    if (midVert < 0) {
        bottom = top = std::max(_height + midVert, 0.0f) / 2.0f;
        midVert = 0;
    }
    float tileSizeY[] = {bottom, midVert, top};

    nsVec2 pos;
    for (int x = 0; x < 3; x++) {
        pos.y = 0;
        for (int y = 0; y < 3; y++) {
            auto &tile = tiles[x][y];
            tile.size = {tileSizeX[x], tileSizeY[y]};

            buffer->AddQuad(pos, tile, nsColor::white);
            pos.y += tile.size.y;
            if (y == 2) {
                pos.x += tile.size.x;
            }
        }
    }
}

void nsSprite9SliceDesc::Draw(IRenDevice *dev) {
    if (!_buff) {
        _buff = new nsQuadsBuffer(dev, 9, false);
        _invalid = true;
    }

    if (_invalid) {
        _invalid = false;
        _buff->Clear();
        AddToBuffer(_buff);
    }

    dev->TextureBind(tex);
    dev->SetColor(color);
    _buff->Draw();
}

void nsSprite9SliceDesc::SetWidth(float width) {
    if (_width != width) {
        _invalid = true;
        _width = width;
        assert(_width >= 0);
    }
}

void nsSprite9SliceDesc::SetHeight(float height) {
    if (_height != height) {
        _invalid = true;
        _height = height;
        assert(_height >= 0);
    }
}

