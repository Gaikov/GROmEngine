//
// Created by Roman on 6/1/2024.
//

#include "InteractiveElement.h"

nsInteractiveElement::nsInteractiveElement() {
    _policies.push_back(this);
}

void nsInteractiveElement::OnKeyUp(int key, int mods) {

}

void nsInteractiveElement::OnKeyDown(int key, bool rept, int mods) {

}

void nsInteractiveElement::OnChar(char ch) {

}

bool nsInteractiveElement::OnPointerUp(const nsPointerData &pointer) {
    const auto pointerId = pointer.pointerId;
    if (!IsEnabled()) {
        return false;
    }

    if (_pointerDown && _pointerDownId == pointerId) {
        _pointerDown = false;
        for (auto p : _policies) {
            p->OnPointerUp();
            p->OnClick();
        }
        return true;
    }
    return false;
}

bool nsInteractiveElement::OnPointerDown(const nsPointerData &pointer) {
    const auto x = pointer.pos.x;
    const auto y = pointer.pos.y;
    const auto pointerId = pointer.pointerId;
    if (!IsEnabled()) {
        return false;
    }

    if (_pointerDown) {
        return true;
    }

    if (HitTest(x, y)) {
        if (!_pointerDown) {
            _pointerDown = true;
            _pointerDownId = pointerId;
            for (auto p : _policies) {
                p->OnPointerDown();
            }
        }
        _pointerDown = true;
        return true;
    }
    return false;
}

bool nsInteractiveElement::OnPointerMove(const nsPointerData &pointer) {
    const auto x = pointer.pos.x;
    const auto y = pointer.pos.y;
    const auto pointerId = pointer.pointerId;
    if (!IsEnabled()) {
        return false;
    }

    auto hit = HitTest(x, y);
    if (hit) {
        if (!_pointerOver) {
            _pointerOver = true;
            _pointerOverId = pointerId;
            for (auto p : _policies) {
                p->OnPointerOver();
            }
        }
        return true;
    } else {
        OnPointerCancel(pointer);
    }
    return false;
}

bool nsInteractiveElement::OnMouseWheel(float delta) {
    return false;
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
                for (auto p : _policies) {
                    p->OnPointerUp();
                }
                _pointerDown = false;
            }

            if (_pointerOver) {
                for (auto p : _policies) {
                    p->OnPointerOut();
                }
                _pointerOver = false;
            }
        }
    }
}

bool nsInteractiveElement::IsEnabled() const {
    return _enabled;
}

void nsInteractiveElement::OnPointerCancel(const nsPointerData &pointer) {
    const auto pointerId = pointer.pointerId;
    if (_pointerOverId == pointerId) {
        if (_pointerOver) {
            for (auto p: _policies) {
                p->OnPointerOut();
            }
            _pointerOver = false;
        }
    }

    if (_pointerDownId == pointerId) {
        if (_pointerDown) {
            for (auto p : _policies) {
                p->OnPointerUp();
            }
            _pointerDown = false;
        }
    }
}

void nsInteractiveElement::AddPolicy(nsInteractivePolicy *p) {
    _policies.push_back(p);
}
