//
// Created by Roman on 9/19/2024.
//

#include "ScrollThumb.h"

nsScrollThumb::nsScrollThumb() {
    desc.tex = _device->TextureLoad("default/ui/frame-white.png");
    desc.SetGrid(6, 6, 6, 6);
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
        desc.color = colorDown;
    } else if (_over) {
        desc.color = colorOver;
    } else {
        desc.color = colorOut;
    }
}

