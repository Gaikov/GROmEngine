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

    if (font) {
        font->Draw(text, labelPos, nsVec2(1, 1), textColor);
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

nsVec2 nsImageButton::ComputeAlignedTextPosition(nsAlign::Type hAlign,
                                                  nsAlign::Type vAlign) const {
    if (!font) {
        return {};
    }

    nsVec2 areaSize;
    if (up.tex) {
        int w, h;
        up.tex->GetSize(w, h);
        areaSize = {static_cast<float>(w), static_cast<float>(h)};
    }

    float size[2];
    font->GetSize(text, size);

    return {
        nsAlign::Compute(hAlign, size[0], areaSize.x),
        nsAlign::Compute(vAlign, size[1], areaSize.y)
    };
}

void nsImageButton::AlignText(nsAlign::Type hAlign, nsAlign::Type vAlign) {
    if (font) {
        labelPos = ComputeAlignedTextPosition(hAlign, vAlign);
    }
}
