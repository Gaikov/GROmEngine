//
// Created by Roman on 2/3/2025.
//

#include "ImageButton.h"
#include "display/align/Align.h"

nsImageButton::nsImageButton() : _currentState(&up) {

}

void nsImageButton::GetLocalBounds(nsRect &bounds) {
    up.GetBounds(bounds);
}

void nsImageButton::Loop() {

}

void nsImageButton::DrawContent(const nsVisualContext2d &context) {
    _device->StateApply(renState);
    _currentState->Draw(_device);

    if (font) {
        nsRect  rect;
        GetLocalBounds(rect);

        float size[2];
        font->GetSize(text, size);

        nsVec2 pos = {
                nsAlign::Compute(nsAlign::CENTER, size[0], rect.width),
                nsAlign::Compute(nsAlign::CENTER, size[1], rect.height),
        };

        font->Draw(text, pos, nsVec2(1, 1), nsColor::white);
    }
}

bool nsImageButton::HitTest(float x, float y) {
    return nsVisualObject2d::HitTest(x, y);
}

void nsImageButton::OnPointerOver() {
    _currentState = &over;
}

void nsImageButton::OnPointerOut() {
    _currentState = &up;
}

void nsImageButton::OnPointerDown() {
    _currentState = &down;
}

void nsImageButton::OnPointerUp() {
    _currentState = &up;
}

void nsImageButton::OnClick() {
    if (onClick) {
        onClick();
    }
}

