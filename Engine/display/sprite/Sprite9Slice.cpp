//
// Created by Roman on 9/16/2024.
//

#include "Sprite9Slice.h"

void nsSprite9Slice::GetLocalBounds(nsRect &bounds) {
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = desc.GetWidth();
    bounds.height = desc.GetHeight();
}

void nsSprite9Slice::Loop() {

}

void nsSprite9Slice::DrawContent(const nsVisualContext2d &context) {
    desc.Draw(_device);
}

void nsSprite9Slice::SetWidth(float width) {
    auto s = origin.scale->x;
    if (s != 0.0f) {
        desc.SetWidth(abs(width / s));
    } else {
        desc.SetWidth(0);
    }
}

void nsSprite9Slice::SetHeight(float height) {
    auto s = origin.scale->y;
    if (s != 0.0f) {
        desc.SetHeight(abs(height / s));
    } else {
        desc.SetHeight(0);
    }
}
