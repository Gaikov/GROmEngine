//
// Created by Roman on 8/8/2024.
//

#include "HGroupLayout.h"

nsHGroupLayout::nsHGroupLayout() : gap(0),
                                   horizontalAlign(nsAlign::BEGIN) {
    gap.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
    horizontalAlign.AddHandler(nsPropChangedName::CHANGED, _onChangeHandler);
}

void nsHGroupLayout::ComputeChildSize(nsVisualObject2d *child) {
    if (HasHorizontalAnchors(child)) {
        ComputeChildWidth(child);
    }
    ComputeChildHeight(child);
}

void nsHGroupLayout::UpdateChildrenPositions() {
    nsRect bounds;
    _boundsCache.clear();
    float childrenWidth = 0;
    int numAlignedChildren = 0;
    auto &children = GetChildren();
    for (const auto child : children) {
        child->GetBounds(bounds, this);
        _boundsCache.push_back(bounds);

        if (!HasHorizontalAnchors(child)) {
            childrenWidth += bounds.width;
            numAlignedChildren ++;
        }
    }
    childrenWidth += static_cast<float>(numAlignedChildren - 1) * gap;

    float startPos = nsAlign::Compute(horizontalAlign, childrenWidth, GetContentWidth());

    for (int i = 0; i < static_cast<int>(children.size()); i++) {
        const auto child = children[i];
        const auto &childBounds = _boundsCache[i];
        ComputeChildYPlacement(child, childBounds);

        if (HasHorizontalAnchors(child)) {
            ComputeChildXPlacement(child, childBounds);
        } else {
            nsVec2 pos = child->origin.pos;
            pos.x = pos.x - childBounds.x + startPos;
            child->origin.pos = pos;
            startPos += (childBounds.width + gap);
        }
    }
}

bool nsHGroupLayout::HasHorizontalAnchors(nsVisualObject2d *child) {
    return child->xMin.IsUsed() || child->xMax.IsUsed() || child->xCenter.IsUsed();
}


