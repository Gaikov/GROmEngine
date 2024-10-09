//
// Created by Roman on 9/4/2024.
//

#include "OrthogonalNativeView.h"

const nsMatrix &nsOrthogonalNativeView::GetViewMatrix() {
    if (!_valid) {
        _valid = true;
        _matrix.SetOrthoDimRH(_width, _height);
    }

    return _matrix;
}

nsVec2 nsOrthogonalNativeView::ScreenToTarget(float x, float y) {
    return {x, _height - y};
}

void nsOrthogonalNativeView::SetScreenSize(float width, float height) {
    if (width != _width) {
        _width = width;
        _valid = false;
    }

    if (height != _height) {
        _height = height;
        _valid = false;
    }
}
