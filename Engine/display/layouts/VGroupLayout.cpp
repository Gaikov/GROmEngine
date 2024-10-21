//
// Created by Roman on 8/8/2024.
//
#include "VGroupLayout.h"

nsVGroupLayout::nsVGroupLayout() : gap(0),
                                   verticalAlign(nsAlign::END) {
    gap.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    verticalAlign.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
}

void nsVGroupLayout::ComputeChildSize(nsVisualObject2d *child) {
    if (HasVerticalAnchors(child)) {
        ComputeChildHeight(child);
    }
    ComputeChildWidth(child);
}

void nsVGroupLayout::UpdateChildrenPositions() {
    nsRect bounds;
    _boundsCache.clear();
    float childrenHeight = 0;
    int numAlignedChildren = 0;
    auto &children = GetChildren();
    for (const auto child : children) {
        child->GetBounds(bounds, this);
        _boundsCache.push_back(bounds);

        if (!HasVerticalAnchors(child)) {
            childrenHeight += bounds.height;
            numAlignedChildren ++;
        }
    }
    childrenHeight += static_cast<float>(numAlignedChildren - 1) * gap;

    float startPos = nsAlign::Compute(verticalAlign, childrenHeight, GetContentHeight());

    for (int i = static_cast<int>(children.size()) - 1; i >= 0; i--) {
        const auto child = children[i];
        const auto &childBounds = _boundsCache[i];
        ComputeChildXPlacement(child, childBounds);

        if (HasVerticalAnchors(child)) {
            ComputeChildYPlacement(child, childBounds);
        } else {
            nsVec2 pos = child->origin.pos;
            pos.y = pos.y - childBounds.y + startPos;
            child->origin.pos = pos;

            startPos += (childBounds.height + gap);
        }
    }
}

bool nsVGroupLayout::HasVerticalAnchors(nsVisualObject2d *child) {
    return child->yMin.IsUsed() || child->yMax.IsUsed() || child->yCenter.IsUsed();
}
