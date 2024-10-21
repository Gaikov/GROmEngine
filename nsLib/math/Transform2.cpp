//
// Created by Roman on 7/24/2024.
//

#include "Transform2.h"

nsTransform2::nsTransform2() : pos(nsVec2(0, 0)),
                               scale(nsVec2(1, 1)),
                               angle(0) {
    _localMatrix.Identity();
    _worldMatrix.Identity();
    _invWorldMatrix.Identity();

    _onChanged = [this](const nsBaseEvent&)-> void {
        _localValid = false;
        InvalidateWorld();
    };

    pos.AddHandler(nsPropChangedName::CHANGED, _onChanged);
    scale.AddHandler(nsPropChangedName::CHANGED, _onChanged);
    angle.AddHandler(nsPropChangedName::CHANGED, _onChanged);
}

const nsMatrix2 & nsTransform2::GetLocal() {
    if (!_localValid) {
        _localValid = true;
        _localMatrix.Transform(pos, scale, angle);
    }

    return _localMatrix;
}

const nsMatrix2 & nsTransform2::GetWorld() {
    if (!_worldValid) {
        _worldValid = true;
        if (_parent) {
            _worldMatrix = GetLocal().Multiply(_parent->GetWorld());
        } else {
            _worldMatrix = GetLocal();
        }
    }
    return _worldMatrix;
}

const nsMatrix2 &nsTransform2::GetWorldInvert() {
    if (!_invWorldValid) {
        _invWorldValid = true;
        if (!GetWorld().Inverse(_invWorldMatrix)) {
            printf("Can't invert matrix!");
        }
    }
    return _invWorldMatrix;
}

void nsTransform2::AddChild(nsTransform2 *child) {
    if (child->_parent == this || _parent == child) {
        return;
    }

    if (_parent) {
        _parent->RemoveChild(child);
    }

    child->_parent = this;
    _children.Add(child);
    child->InvalidateWorld();
}

void nsTransform2::RemoveChild(nsTransform2 *child) {
    if (child->_parent == this) {
        _children.Remove(child);
        child->_parent = nullptr;
        child->InvalidateWorld();
    }
}

void nsTransform2::InvalidateWorld() {
    _worldValid = false;
    _invWorldValid = false;
    for (auto child = _children.GetHead(); child; child = child->NextItem()) {
        child->InvalidateWorld();
    }
}

nsVec2 nsTransform2::ToLocal(const nsVec2 &global) {
    return GetWorldInvert().TransformPoint(global);
}

nsVec2 nsTransform2::ToGlobal(const nsVec2 &local) {
    return GetWorld().TransformPoint(local);
}

