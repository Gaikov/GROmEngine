//
// Created by Roman on 6/1/2024.
//

#include "InteractiveElement.h"

void nsInteractiveElement::OnKeyUp(int key) {

}

void nsInteractiveElement::OnKeyDown(int key, bool rept) {

}

void nsInteractiveElement::OnChar(char ch) {

}

bool nsInteractiveElement::OnPointerUp(float x, float y, int pointerId) {
    if (!IsEnabled()) {
        return false;
    }

    if (_pointerDown && _pointerId == pointerId) {
        _pointerDown = false;
        OnPointerUp();
        OnClick();
        return true;
    }
    return false;
}

bool nsInteractiveElement::OnPointerDown(float x, float y, int pointerId) {
    if (!IsEnabled()) {
        return false;
    }

    if (_pointerDown) {
        return true;
    }

    if (HitTest(x, y)) {
        if (!_pointerDown) {
            _pointerDown = true;
            _pointerId = pointerId;
            OnPointerDown();
        }
        _pointerDown = true;
        return true;
    }
    return false;
}

bool nsInteractiveElement::OnPointerMove(float x, float y, int pointerId) {
    if (!IsEnabled()) {
        return false;
    }

    auto hit = HitTest(x, y);
    if (hit) {
        if (!_pointerOver) {
            _pointerOver = true;
            _pointerId = pointerId;
            OnPointerOver();
        }
        return true;
    } else {
        OnPointerCancel(pointerId);
    }
    return false;
}

void nsInteractiveElement::OnMouseWheel(float delta) {

}

bool nsInteractiveElement::IsPointerOver() const {
    return _pointerOver;
}

bool nsInteractiveElement::IsPointerDown() const {
    return _pointerDown;
}

void nsInteractiveElement::SetEnabled(bool enabled) {
    if (_enabled != enabled) {
        _enabled = enabled;
        if (!_enabled) {
            if (_pointerDown) {
                OnPointerUp();
                _pointerDown = false;
            }

            if (_pointerOver) {
                OnPointerOut();
                _pointerOver = false;
            }
        }
    }
}

bool nsInteractiveElement::IsEnabled() const {
    return _enabled;
}

void nsInteractiveElement::OnPointerCancel(int pointerId) {
    if (_pointerId == pointerId) {
        if (_pointerOver) {
            OnPointerOut();
            _pointerOver = false;
        }

        if (_pointerDown) {
            OnPointerUp();
            _pointerDown = false;
        }
    }
}
