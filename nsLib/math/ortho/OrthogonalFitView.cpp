//
// Created by Roman on 6/3/2024.
//

#include "OrthogonalFitView.h"

nsOrthogonalFitView::nsOrthogonalFitView() : _matrix(1) {

}

void nsOrthogonalFitView::SetScreenSize(float width, float height) {
    if (_screenWidth != width) {
        _valid = false;
        _screenWidth = width;
    }

    if (_screenHeight != height) {
        _valid = false;
        _screenHeight = height;
    }

    if (!_valid) {
        ComputeTargetViewport();
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

    if (!_valid) {
        ComputeTargetViewport();
    }
}

const nsMatrix &nsOrthogonalFitView::GetViewMatrix() {
    if (!_valid) {
        _valid = true;

        float aspectView = _screenWidth / _screenHeight;
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

    y = _screenHeight - y;

    float scaleX = _targetWidth / _screenWidth;
    float scaleY = _targetHeight / _screenHeight;
    float scale = std::max(scaleX, scaleY);

    float offsX = (_screenWidth * scale - _targetWidth) / 2;
    float offsY = (_screenHeight * scale - _targetHeight) / 2;

    res.x = x * scale - offsX;
    res.y = y * scale - offsY;

    return res;
}


void nsOrthogonalFitView::ComputeTargetViewport() {
    const float aspectView = _screenWidth / _screenHeight;
    const float aspectTarget = _targetWidth / _targetHeight;

    nsVec2 exts;

    if (aspectView > aspectTarget) {
        exts.x = (_targetHeight * aspectView - _targetWidth) / 2;
    } else {
        exts.y = (_targetWidth / aspectView - _targetHeight) / 2;
    }

    _targetViewport.x = -exts.x;
    _targetViewport.y = -exts.y;
    _targetViewport.width = _targetWidth + exts.x * 2;
    _targetViewport.height = _targetHeight + exts.y * 2;
}
