//
// Created by Roman on 2/3/2025.
//

#include "ImageButton.h"

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

