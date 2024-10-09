//
// Created by Roman on 8/1/2024.
//

#include "GroupLayout.h"

void nsGroupLayout::ComputeChildSize(nsVisualObject2d *child) {
    ComputeChildWidth(child);
    ComputeChildHeight(child);
}

void nsGroupLayout::ComputeChildWidth(nsVisualObject2d *child) const {
    auto width = GetContentWidth();

    if (!child->xCenter.IsUsed()) {
        if (child->xMin.IsUsed() && child->xMax.IsUsed()) {
            auto min = child->xMin.Compute(width);
            auto max = width - child->xMax.Compute(width);
            child->SetWidth(max - min);
        }
    }
}

void nsGroupLayout::ComputeChildHeight(nsVisualObject2d *child) const {
    auto height = GetContentHeight();

    if (!child->yCenter.IsUsed()) {
        if (child->yMin.IsUsed() && child->yMax.IsUsed()) {
            auto min = child->yMin.Compute(height);
            auto max = height - child->yMax.Compute(height);
            child->SetHeight(max - min);
        }
    }
}

void nsGroupLayout::ComputeChildPlacement(nsVisualObject2d *child) {
    nsRect  bounds;
    child->GetBounds(bounds, this);

    ComputeChildXPlacement(child, bounds);
    ComputeChildYPlacement(child, bounds);
}

void nsGroupLayout::ComputeChildXPlacement(nsVisualObject2d *child, const nsRect &bounds) const {
    auto width = GetContentWidth();

    nsVec2 pos = child->origin.pos;
    if (child->xCenter.IsUsed()) {
        //offset to zero + offset to center + anchor offset from center
        pos.x = pos.x - bounds.x + (width - bounds.width) / 2 + child->xCenter.Compute(width);
    } else if (child->xMin.IsUsed()) {
        pos.x = pos.x - bounds.x + child->xMin.Compute( width);
    } else if (child->xMax.IsUsed()) {
        pos.x += (width - bounds.maxX());
        pos.x -= child->xMax.Compute(width);
    }
    child->origin.pos = pos;
}

void nsGroupLayout::ComputeChildYPlacement(nsVisualObject2d *child, const nsRect &bounds) const {
    auto height = GetContentHeight();
    nsVec2 pos = child->origin.pos;
    if (child->yCenter.IsUsed()) {
        pos.y = pos.y - bounds.y + (height - bounds.height) / 2 + child->yCenter.Compute(height);
    } else if (child->yMin.IsUsed()) {
        pos.y = pos.y - bounds.y + child->yMin.Compute( height);
    } else if (child->yMax.IsUsed()) {
        pos.y += (height - bounds.maxY());
        pos.y -= child->yMax.Compute(height);
    }
    child->origin.pos = pos;
}