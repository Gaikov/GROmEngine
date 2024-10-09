//
// Created by Roman on 5/31/2024.
//

#include "TileDef.h"
#include "RenDevice.h"

void nsTileDef::Compute(ITexture *tex, const nsVec2 &texturePos, const nsVec2 &rectSize) {
    Compute(tex, texturePos.x, texturePos.y, rectSize.x, rectSize.y);
}

void nsTileDef::Compute(ITexture *tex, float x, float y, float width, float height) {
    size = nsVec2(width, height);

    int w = 1, h = 1;
    if (tex) {
        tex->GetSize(w, h);
    }
    auto texWidth = (float)w;
    auto texHeight = (float)h;

    uvStart.x = x / texWidth;
    uvStart.y = y / texHeight;

    uvEnd.x = (x + width) / texWidth;
    uvEnd.y = (y + height) / texHeight;
}

void nsTileDef::Compute(ITexture *tex) {
    int w = 1, h = 1;
    if (tex) {
        tex->GetSize(w, h);
    }
    size = nsVec2((float)w, (float)h);
    uvStart = nsVec2(0, 0);
    uvEnd = nsVec2(1, 1);
}
