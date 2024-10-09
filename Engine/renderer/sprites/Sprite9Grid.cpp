//
// Created by Roman on 5/31/2024.
//

#include "Sprite9Grid.h"
#include "RenDevice.h"
#include "renderer/QuadsBuffer.h"
#include "nsLib/log.h"

void nsSprite9Grid::SetTexture(ITexture *t) {
    _tex = t;
    _invalid = true;
}

void nsSprite9Grid::SetGrid(float leftPad, float topPad, float rightPad, float bottomPad) {
    _leftPad = leftPad;
    _rightPad = rightPad;
    _topPad = topPad;
    _bottomPad = bottomPad;
    _invalid = true;
}

void nsSprite9Grid::AddToBuffer(nsQuadsBuffer *buffer) {
    if (!_tex) {
        Log::Warning("Texture is not set for Sprite9Grid!");
        return;
    }

    int w, h;
    _tex->GetSize(w, h);

    auto texWidth = (float) w;
    auto texHeight = (float) h;

    auto midHorz = texWidth - (_leftPad + _rightPad);
    auto midVert = texHeight - (_topPad + _bottomPad);
    auto right = texWidth - _leftPad;
    auto bottom = texHeight - _bottomPad;
    auto left = _leftPad;
    auto top = _topPad;

    float posX[] = {0, left, right};
    float sizeX[] = {_leftPad, midHorz, _rightPad};
    float posY[] = {bottom, top, 0};
    float sizeY[] = {_bottomPad, midVert, top};

    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            _tiles[x][y].Compute(_tex, posX[x], posY[y], sizeX[x], sizeY[y]);
        }
    }

    nsVec2 pos;
    for (int x = 0; x < _gridWidth; x++) {
        pos.y = 0;
        for (int y = 0; y < _gridHeight; y++) {
            auto &tile = _tiles[GetTileIndex(x, _gridWidth)][GetTileIndex(y, _gridHeight)];

            buffer->AddQuad(pos, tile, color);
            pos.y += tile.size.y;

            if (y == _gridHeight - 1) {
                pos.x += tile.size.x;
            }
        }
    }
}

void nsSprite9Grid::Draw(IRenDevice *dev) {
    if (!_buff) {
        _buff = new nsQuadsBuffer(dev, 100 * 100);
    }

    if (_invalid && _tex) {
        Log::Debug("Validate Sprite9Grid buffer");
        _invalid = false;
        _buff->Clear();

        AddToBuffer(_buff);
    }

    dev->TextureBind(_tex);
    dev->SetColor(color);
    _buff->Draw();
}

nsSprite9Grid::~nsSprite9Grid() {
    delete _buff;
}

int nsSprite9Grid::GetGridWidth() const {
    return _gridWidth;
}

void nsSprite9Grid::SetGridWidth(int value) {
    value = std::max(2, value);
    if (_gridWidth != value) {
        _invalid = true;
        _gridWidth = value;
    }
}

int nsSprite9Grid::GetGridHeight() const {
    return _gridHeight;
}

void nsSprite9Grid::SetGridHeight(int value) {
    value = std::max(2, value);
    if (_gridHeight != value) {
        _invalid = true;
        _gridHeight = value;
    }
}

int nsSprite9Grid::GetTileIndex(int pos, int size) {
    if (!pos) {
        return 0;
    } else if (pos == size - 1) {
        return 2;
    }
    return 1;
}

float nsSprite9Grid::GetWidth() {
    if (!_tex) {
        return 0;
    }

    int w, h;
    _tex->GetSize(w, h);
    float midSize = (float)w - _leftPad - _rightPad;

    return midSize * float(_gridWidth - 2) + _leftPad + _rightPad;
}

float nsSprite9Grid::GetHeight() {
    if (!_tex) {
        return 0;
    }

    int w, h;
    _tex->GetSize(w, h);
    float midSize = (float)h - _topPad - _bottomPad;

    return midSize * float(_gridHeight - 2) + _topPad + _bottomPad;
}

ITexture *nsSprite9Grid::GetTexture() {
    return _tex;
}