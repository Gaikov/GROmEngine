//
// Created by Roman on 6/3/2024.
//

#include "OrthogonalFitView.h"

nsOrthogonalFitView::nsOrthogonalFitView() : _matrix(1) {

}

void nsOrthogonalFitView::SetScreenSize(float width, float height) {
    if (_viewWidth != width) {
        _valid = false;
        _viewWidth = width;
    }

    if (_viewHeight != height) {
        _valid = false;
        _viewHeight = height;
    }
}

void nsOrthogonalFitView::SetTargetSize(float width, float height) {

    if (_targetWidth != width) {
        _valid = false;
        _targetWidth = width;
    }

    if (_targetHeight != height) {
        _valid = false;
        _targetHeight = height;
    }
}

const nsMatrix &nsOrthogonalFitView::GetViewMatrix() {
    if (!_valid) {
        _valid = true;

        float aspectView = _viewWidth / _viewHeight;
        float aspectTarget = _targetWidth / _targetHeight;
        nsMatrix scale(1);
        if (aspectView > aspectTarget) {
            scale.Scale( aspectTarget / aspectView, 1, 1);
        } else {
            scale.Scale( 1, aspectView / aspectTarget, 1);
        }

        _matrix.SetOrthoDimRH(_targetWidth, _targetHeight);
        _matrix = _matrix * scale;
    }
    return _matrix;
}

nsVec2 nsOrthogonalFitView::ScreenToTarget(float x, float y) {
    nsVec2 res;

    y = _viewHeight - y;

    float scaleX = _targetWidth / _viewWidth;
    float scaleY = _targetHeight / _viewHeight;
    float scale = std::max(scaleX, scaleY);

    float offsX = (_viewWidth * scale - _targetWidth) / 2;
    float offsY = (_viewHeight * scale - _targetHeight) / 2;

    res.x = x * scale - offsX;
    res.y = y * scale - offsY;

    return res;
}

