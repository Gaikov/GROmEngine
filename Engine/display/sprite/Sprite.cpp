//
// Created by Roman on 8/6/2024.
//

#include "Sprite.h"
#include "RenAux.h"

nsSprite::nsSprite() {
    desc.size = {100, 100};
}

void nsSprite::GetLocalBounds(nsRect &bounds) {
    nsVec2 pos = -desc.center;
    bounds.x = pos.x;
    bounds.y = pos.y;
    bounds.width = desc.size.x;
    bounds.height = desc.size.y;
}

void nsSprite::Loop() {

}

void nsSprite::DrawContent(const nsVisualContext2d &context) {
    if (renState) {
        _device->StateApply(renState);
    }
    desc.Draw(_device);

    if (drawFrame) {
        nsRect rect;
        GetLocalBounds(rect);
        nsGizmos::DrawRect(rect, nsColor::white);
    }
}

