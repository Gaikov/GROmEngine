//
// Created by Roman on 9/19/2024.
//

#include "ScrollThumb.h"

nsScrollThumb::nsScrollThumb() {
    tex = _device->TextureLoad("default/ui/frame-white.png");
    gridLeft = 6;
    gridTop = 6;
    gridRight = 6;
    gridBottom = 6;
    UpdateState();
}

bool nsScrollThumb::HitTest(float x, float y) {
    return nsVisualObject2d::HitTest(x, y);
}

void nsScrollThumb::OnPointerOver() {
    _over = true;
    UpdateState();
}

void nsScrollThumb::OnPointerOut() {
    _over = false;
    UpdateState();
}

void nsScrollThumb::OnPointerDown() {
    _down = true;
    UpdateState();
}

void nsScrollThumb::OnPointerUp() {
    _down = false;
    UpdateState();
}

void nsScrollThumb::OnClick() {

}

void nsScrollThumb::UpdateState() {
    if (_down) {
        color = colorDown;
    } else if (_over) {
        color = colorOver;
    } else {
        color = colorOut;
    }
}

