//
// Created by Roman on 7/24/2024.
//

#pragma once

#include "Matrix2.h"
#include "nsLib/Vec2.h"
#include "nsLib/models/Property.h"
#include "nsLib/structs/FastList.h"

class nsTransform2 : public nsListItem<nsTransform2> {
public:
    nsProperty<nsVec2> pos;
    nsProperty<nsVec2> scale;
    nsProperty<float> angle;

    nsTransform2();
    const nsMatrix2 &GetLocal();
    const nsMatrix2 &GetWorld();
    const nsMatrix2 &GetWorldInvert();

    nsVec2 ToLocal(const nsVec2 &global);
    nsVec2 ToGlobal(const nsVec2 &local);

    void AddChild(nsTransform2 *child);
    void RemoveChild(nsTransform2 *child);
    nsTransform2* GetParent() const { return _parent; }
    void Reset();

    float ToGlobalAngle(float localAngle) const;
    float ToLocalAngle(float globalAngle) const;

private:
    nsFastList<nsTransform2>    _children;

    nsTransform2 *_parent = nullptr;
    bool _localValid = true;
    bool _worldValid = true;
    bool _invWorldValid = true;
    nsMatrix2 _localMatrix;
    nsMatrix2 _worldMatrix;
    nsMatrix2 _invWorldMatrix;

    void InvalidateWorld();
};
